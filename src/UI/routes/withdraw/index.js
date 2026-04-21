/* eslint-disable curly */
import template from "./template.js"
import Route from "/core/UI/Route.js"
import { Access } from "/core/Access.js"
import { Wallets, Chains } from "/core/Stores.js"
import { Context } from "/core/Context.js"
import { notify } from "/core/Utils.js"
import SendLogic from "./logic.js"

export class WITHDRAW extends Route {
    static module = import.meta.url

    constructor() {
        super(template)
        this.estimateGas = this.estimateGas.bind(this)
        this.submit = this.submit.bind(this)
    }

    async onconnect() {
        // ── DOM refs ──────────────────────────────────────────────
        this.$wallets = this.shadowRoot.querySelector("#wallets")
        this.$withdrawCard = this.shadowRoot.querySelector("#withdraw-card")
        this.$form = this.shadowRoot.querySelector("#form")
        this.$gas = this.shadowRoot.querySelector("#gas")
        this.$submit = this.shadowRoot.querySelector("#submit")
        this.$addressError = this.shadowRoot.querySelector("#address-error")
        this.$amountSymbol = this.shadowRoot.querySelector("#amount-symbol")
        this.$availableBalance = this.shadowRoot.querySelector("#available-balance")
        this.$amountError = this.shadowRoot.querySelector("#amount-error")
        this.$infoNetwork = this.shadowRoot.querySelector("#info-network-value")
        this.$infoAddress = this.shadowRoot.querySelector("#info-address-value")

        // ── Form inputs ───────────────────────────────────────────
        this.$addressDisplay = this.$form.querySelector("#address-display")
        this.$addressHidden = this.$form.querySelector("#address-hidden")
        const $amount = this.$form.querySelector("input[name='amount']")

        this.listen(this.$addressDisplay, "input", () => {
            this._syncAddressValue()
            this._syncSubmit()
            this.estimateGas()
        })
        this.listen(this.$addressDisplay, "paste", (e) => {
            e.preventDefault()
            const text = (e.clipboardData || window.clipboardData).getData("text/plain").trim()
            const sel = this.shadowRoot.getSelection?.() ?? window.getSelection()
            if (sel && sel.rangeCount) {
                sel.deleteFromDocument()
                sel.getRangeAt(0).insertNode(document.createTextNode(text))
                sel.collapseToEnd()
            } else {
                this.$addressDisplay.textContent += text
            }
            this.$addressDisplay.dispatchEvent(new Event("input", { bubbles: true }))
        })
        this.listen(this.$addressDisplay, "blur", () => this._syncAddressError())
        this.listen(this.$addressDisplay, "keydown", (e) => {
            if (e.key === "Enter") e.preventDefault()
        })

        this.listen($amount, "input", () => {
            this._syncAmountError()
            this._syncSubmit()
            this.estimateGas()
        })

        this.listen(this.$submit, "click", this.submit)

        // ── Auth gate ─────────────────────────────────────────────
        this.watch(Access, "authenticated", ({ value }) => {
            this._syncSubmit()
            if (!value) {
                this.$form.reset()
                this.$gas.removeAttribute("visible")
                this.$gas.textContent = ""
            }
        })

        // ── Wallets state changes ─────────────────────────────────
        this.sub(
            this.$wallets.states.on("currency", () => {
                this._syncAmountSymbol()
                this._syncNetworkInfo()
                this._syncFormVisibility()
                this._syncSubmit()
                this.estimateGas()
            }),
            this.$wallets.states.on("chain", () => {
                this._syncNetworkInfo()
                this._syncFormVisibility()
                this._syncSubmit()
                this.estimateGas()
            }),
            this.$wallets.states.on("address", () => {
                this._syncNetworkInfo()
            }),
            this.$wallets.states.on("balance", () => {
                this._syncAvailableBalance()
                this._syncAmountError()
            })
        )

        // ── Step sequencing on load ───────────────────────────────
        // Wallets component seeds chain/currency from URL params on its own;
        // run an initial sync once it has settled.
        this._syncAmountSymbol()
        this._syncAvailableBalance()
        this._syncNetworkInfo()
        this._syncFormVisibility()
        this._syncSubmit()
    }

    ondisconnect() {
        clearTimeout(this.$gaspend)
    }

    // ── Sync helpers ──────────────────────────────────────────────

    _syncFormVisibility() {
        const ready = !!(this.$wallets.states.get("currency") && this.$wallets.states.get("chain"))
        this.$withdrawCard.toggleAttribute("visible", ready)
        if (!ready) {
            this.$form.reset()
            if (this.$addressDisplay) this.$addressDisplay.textContent = ""
            this.$gas.removeAttribute("visible")
            this.$gas.textContent = ""
            if (this.$addressError) this.$addressError.removeAttribute("visible")
            if (this.$amountError) this.$amountError.removeAttribute("visible")
            if (this.$infoNetwork) this.$infoNetwork.textContent = "—"
            if (this.$infoAddress) this.$infoAddress.textContent = "—"
        }
    }

    _syncAmountSymbol() {
        const currency = this.$wallets.states.get("currency")
        if (this.$amountSymbol) this.$amountSymbol.textContent = currency || ""
    }

    _syncAmountError() {
        if (!this.$amountError) return
        const amount = Number(this.$form?.querySelector("input[name='amount']")?.value)
        const balance = this.$wallets.states.get("balance")
        const exceeded = amount > 0 && balance != null && amount > Number(balance.replace(/[^0-9.]/g, ""))
        this.$amountError.toggleAttribute("visible", exceeded)
    }

    _syncNetworkInfo() {
        const chainId = this.$wallets.states.get("chain")
        const chain = Chains[chainId]
        if (this.$infoNetwork) {
            if (chain) {
                const currency = this.$wallets.states.get("currency")
                const std = currency ? Object.values(chain.currencies).find(c => c.name === currency)?.ABI : null
                this.$infoNetwork.textContent = std ? `${chain.configs?.name || chainId} (${std})` : chain.configs?.name || chainId
            } else {
                this.$infoNetwork.textContent = "—"
            }
        }
        if (this.$infoAddress) {
            const address = this.$wallets.states.get("address")
            this.$infoAddress.textContent = address || "—"
        }
    }

    _syncAvailableBalance() {
        if (!this.$availableBalance) return
        const balance = this.$wallets.states.get("balance")
        const currency = this.$wallets.states.get("currency")
        if (balance != null && currency) {
            const label = Context.get(["dictionary", "available"]) || "Available"
            this.$availableBalance.textContent = `${label}: ${balance} ${currency}`
            this.$availableBalance.setAttribute("visible", "")
        } else {
            this.$availableBalance.removeAttribute("visible")
            this.$availableBalance.textContent = ""
        }
    }

    _syncAddressValue() {
        const raw = this.$addressDisplay.textContent
        this.$addressHidden.value = raw.trim()
        this._renderAddressHighlight(raw)
    }

    _renderAddressHighlight(raw) {
        if (!raw) return
        const n = raw.length

        // Save caret offset before DOM rewrite
        const sel = window.getSelection()
        let caretOffset = n
        if (sel && sel.rangeCount) {
            const r = sel.getRangeAt(0)
            if (this.$addressDisplay.contains(r.startContainer)) {
                // walk text nodes to accumulate offset
                let offset = 0
                const walker = document.createTreeWalker(this.$addressDisplay, NodeFilter.SHOW_TEXT)
                let node
                while ((node = walker.nextNode())) {
                    if (node === r.startContainer) { caretOffset = offset + r.startOffset; break }
                    offset += node.length
                }
            }
        }

        if (n <= 12) {
            this.$addressDisplay.innerHTML = `<span class="addr-hi">${raw}</span>`
        } else {
            const head = raw.slice(0, 6)
            const mid = raw.slice(6, n - 6)
            const tail = raw.slice(n - 6)
            this.$addressDisplay.innerHTML =
                `<span class="addr-hi">${head}</span>${mid}<span class="addr-hi">${tail}</span>`
        }

        // Restore caret at saved offset
        const range = document.createRange()
        let remaining = caretOffset
        const walker = document.createTreeWalker(this.$addressDisplay, NodeFilter.SHOW_TEXT)
        let node
        while ((node = walker.nextNode())) {
            if (remaining <= node.length) { range.setStart(node, remaining); break }
            remaining -= node.length
        }
        range.collapse(true)
        sel?.removeAllRanges()
        sel?.addRange(range)
    }

    _syncSubmit() {
        const amount = Number(this.$form?.querySelector("input[name='amount']")?.value)
        const address = this.$addressHidden?.value?.trim()
        const currency = this.$wallets.states.get("currency")
        const chain = this.$wallets.states.get("chain")
        const balance = this.$wallets.states.get("balance")
        const exceeded = amount > 0 && balance != null && amount > Number(balance.replace(/[^0-9.]/g, ""))
        const ready = !!(currency && chain && Access.get("authenticated") && amount > 0 && address && !exceeded)
        this.$submit?.toggleAttribute("disabled", !ready)
    }

    _syncAddressError() {
        const empty = !this.$addressHidden?.value?.trim()
        if (this.$addressError) this.$addressError.toggleAttribute("visible", empty)
    }

    // ── Gas estimate ──────────────────────────────────────────────

    estimateGas() {
        clearTimeout(this.$gaspend)
        this.$gaspend = setTimeout(async () => {
            const chain = this.$wallets.states.get("chain")
            const currency = this.$wallets.states.get("currency")
            const fee = await SendLogic.gas({
                wallet: Wallets[chain],
                name: currency,
                to: this.$addressHidden.value,
                amount: this.$form.querySelector("input[name='amount']").value
            })
            if (!fee) {
                this.$gas.removeAttribute("visible")
                this.$gas.textContent = ""
                return
            }
            const label = Context.get(["dictionary", "gasFee"])
            this.$gas.textContent = `${label}: ${fee.amount} ${fee.symbol}`
            this.$gas.setAttribute("visible", "")
        }, 500)
    }

    // ── Submit ────────────────────────────────────────────────────

    async submit() {
        const data = Object.fromEntries(new FormData(this.$form))
        const chain = this.$wallets.states.get("chain")
        const currency = this.$wallets.states.get("currency")

        if (!data.amount || !data.address || !chain || !currency) return notify({ content: Context.get(["dictionary", "missingRequiredFields"]), autoClose: true })

        this.$form.querySelectorAll("input").forEach((el) => (el.disabled = true))
        this.$submit.toggleAttribute("disabled", true)

        const result = await SendLogic.send({
            wallet: Wallets[chain],
            name: currency,
            to: data.address,
            amount: data.amount
        })

        this.$form.querySelectorAll("input").forEach((el) => (el.disabled = false))
        this.$submit.toggleAttribute("disabled", false)

        if (result.success) {
            notify({ content: Context.get(["dictionary", "transactionSent"]), autoClose: true })
            this.$form.reset()
            this.$addressDisplay.textContent = ""
            this.$gas.removeAttribute("visible")
            this.$gas.textContent = ""
        } else notify({ content: Context.get(["dictionary", result.error || "transactionError"]), autoClose: true })
    }
}

customElements.define("route-withdraw", WITHDRAW)
export default WITHDRAW
