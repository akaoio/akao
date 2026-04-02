import template from "./template.js"
import { render } from "/core/UI.js"
import { Context } from "/core/Context.js"
import { events } from "/core/Events.js"
import { Lives, Chains, Dexs, Wallets, Elements } from "/core/Stores.js"
import { fiatValue } from "/core/Utils/contracts.js"
import { formatNumber, notify } from "/core/Utils.js"

export class SWAP extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
        this.quote = this.quote.bind(this)
        this.submit = this.submit.bind(this)
    }

    connectedCallback() {
        this.$wallets = this.shadowRoot.querySelector("ui-wallets")
        this.$fromToken = this.shadowRoot.querySelector("#from-token")
        this.$toToken = this.shadowRoot.querySelector("#to-token")
        this.$amountIn = this.shadowRoot.querySelector("#amount-in")
        this.$quoteOut = this.shadowRoot.querySelector("#quote-out")
        this.$balanceIn = this.shadowRoot.querySelector("#balance-in")
        this.$slippage = this.shadowRoot.querySelector("#slippage")
        this.$gas = this.shadowRoot.querySelector("#gas")
        this.$error = this.shadowRoot.querySelector("#error")
        this.$submit = this.shadowRoot.querySelector("#submit")

        this.$amountIn.addEventListener("input", this.quote)
        this.$submit.addEventListener("click", this.submit)

        this._onFromSelect = (e) => {
            this._from = e.detail
            this.quote()
            this.updatebalance()
        }
        this._onToSelect = (e) => {
            this._to = e.detail
            this.quote()
        }
        this.$fromToken.addEventListener("select", this._onFromSelect)
        this.$toToken.addEventListener("select", this._onToSelect)

        this.subscriptions.push(
            this.$wallets.states.on("address", ({ value }) => {
                const active = !!value
                this.$amountIn.disabled = !active
                this.$submit.toggleAttribute("disabled", !active)
                if (!active) {
                    this.$quoteOut.textContent = ""
                    this.$gas.textContent = ""
                    this.$error.textContent = ""
                } else {
                    this.buildoptions()
                }
            }, true),
            this.$wallets.states.on("chain", () => this.buildoptions()),
            events.on("Lives.pools", () => this.buildoptions()),
            () => {
                this.$amountIn.removeEventListener("input", this.quote)
                this.$submit.removeEventListener("click", this.submit)
                this.$fromToken.removeEventListener("select", this._onFromSelect)
                this.$toToken.removeEventListener("select", this._onToSelect)
            }
        )

        // Pre-fill from URL params
        const params = Context.get("params") || {}
        if (params.from) this._prefillFrom = params.from
        if (params.to) this._prefillTo = params.to

        Elements.Access?.checkpoint()
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }

    buildoptions() {
        const chain = this.$wallets.states.get("chain")
        if (!chain) return

        const chainObj = Chains[chain]
        const options = []

        // Currencies already loaded by ui-wallets
        if (chainObj?.currencies) {
            for (const [address, contract] of Object.entries(chainObj.currencies)) {
                options.push({ address, configs: contract })
            }
        }

        // Add tokens from pools not yet in currencies
        const pools = Lives.pools?.[chain]
        if (pools) {
            const known = new Set(options.map((o) => o.address))
            for (const pool of Object.values(pools)) {
                for (const token of [pool.token0, pool.token1]) {
                    if (token?.address && !known.has(token.address) && token.configs) {
                        options.push({ address: token.address, configs: token.configs })
                        known.add(token.address)
                    }
                }
            }
        }

        this.$fromToken.setoptions(options)
        this.$toToken.setoptions(options)

        // Restore pre-filled tokens from URL
        if (this._prefillFrom) {
            const found = options.find((o) => o.address.toLowerCase() === this._prefillFrom.toLowerCase())
            if (found) { this._from = found; this.$fromToken.states.set({ selected: found }) }
            this._prefillFrom = null
        }
        if (this._prefillTo) {
            const found = options.find((o) => o.address.toLowerCase() === this._prefillTo.toLowerCase())
            if (found) { this._to = found; this.$toToken.states.set({ selected: found }) }
            this._prefillTo = null
        }
    }

    finddex(from, to) {
        const chain = this.$wallets.states.get("chain")
        if (!chain || !from || !to) return null

        const pools = Lives.pools?.[chain]
        if (!pools) return null

        for (const [address, pool] of Object.entries(pools)) {
            const isMatch =
                (pool.token0?.address === from && pool.token1?.address === to) ||
                (pool.token0?.address === to && pool.token1?.address === from)
            if (!isMatch) continue

            const dexid = `${chain}.${pool.dex}${pool.version}`
            const dex = Dexs[dexid]
            if (!dex) continue

            return { dex, pool, address }
        }
        return null
    }

    async updatebalance() {
        const chain = this.$wallets.states.get("chain")
        const from = this._from
        if (!chain || !from) return

        const balance = Lives.balances?.[chain]?.[from.address]
        if (balance !== undefined) {
            const label = Context.get(["dictionary", "balance"]) || "Balance"
            this.$balanceIn.textContent = `${label}: ${formatNumber(balance)} ${from.configs?.name || ""}`
        }
    }

    quote() {
        clearTimeout(this._quotePending)
        this._quotePending = setTimeout(() => this.runquote(), 500)
    }

    async runquote() {
        this.$error.textContent = ""
        this.$quoteOut.textContent = ""
        this.$gas.textContent = ""

        const from = this._from
        const to = this._to
        const amount = Number(this.$amountIn.value)
        const chain = this.$wallets.states.get("chain")

        if (!from || !to || !amount || !chain) return

        // Balance check
        const balance = Lives.balances?.[chain]?.[from.address]
        if (balance !== undefined && amount > Number(balance)) {
            this.$error.textContent = Context.get(["dictionary", "insufficientBalance"]) || "Insufficient balance"
            this.$submit.setAttribute("disabled", "")
            return
        }

        const found = this.finddex(from.address, to.address)
        if (!found) {
            this.$error.textContent = Context.get(["dictionary", "nopoolFound"]) || "No pools found"
            this.$submit.setAttribute("disabled", "")
            return
        }

        const { dex, pool } = found
        let result

        try {
            if (pool.version === "V2") {
                result = await dex.getAmountsOut({ token0: from.address, token1: to.address, amount })
            } else if (pool.version === "V3") {
                result = await dex.quote({ token0: from.address, token1: to.address, fee: pool.fee, amountIn: amount })
            }
        } catch (e) {
            this.$error.textContent = e?.message || String(e)
            return
        }

        if (!result) return

        const amountOut = result.token1?.quantity ?? result.amount ?? 0
        const fiat = Context.get("fiat")?.code || "USD"
        const fiatOut = await fiatValue({ chain, currency: to.configs, amount: amountOut, fiat })
        const fiatLabel = fiatOut ? ` (≈ ${fiat} ${formatNumber(fiatOut, 2)})` : ""
        this.$quoteOut.textContent = `${formatNumber(amountOut)} ${to.configs?.name || ""}${fiatLabel}`

        this.$submit.removeAttribute("disabled")

        // Gas estimate
        try {
            const walletAddress = this.$wallets.states.get("address")
            const wallet = Wallets[chain]
            if (wallet && walletAddress) {
                const fee = await wallet.fee({ to: pool.address, amount, currency: from.configs })
                if (fee) {
                    const label = Context.get(["dictionary", "gasFee"]) || "Gas fee"
                    this.$gas.textContent = `${label}: ${fee.amount} ${fee.symbol}`
                }
            }
        } catch (_) { /* non-fatal */ }
    }

    async submit() {
        const from = this._from
        const to = this._to
        const amount = Number(this.$amountIn.value)
        const slippage = Number(this.$slippage.value) || 0.5
        const chain = this.$wallets.states.get("chain")

        if (!from || !to || !amount || !chain) {
            notify({ content: Context.get(["dictionary", "missingRequiredFields"]), autoClose: true })
            return
        }

        const balance = Lives.balances?.[chain]?.[from.address]
        if (balance !== undefined && amount > Number(balance)) {
            notify({ content: Context.get(["dictionary", "insufficientBalance"]), autoClose: true })
            return
        }

        const found = this.finddex(from.address, to.address)
        if (!found) {
            notify({ content: Context.get(["dictionary", "nopoolFound"]), autoClose: true })
            return
        }

        const { dex, pool } = found

        this.$submit.setAttribute("disabled", "")
        this.$amountIn.disabled = true

        try {
            const tx = await dex.swap({ token0: from.address, token1: to.address, amount0: amount, slippage, fee: pool?.fee })
            if (tx && !tx.error) {
                notify({ content: Context.get(["dictionary", "transactionSent"]), autoClose: true })
                this.$amountIn.value = ""
                this.$quoteOut.textContent = ""
                this.$gas.textContent = ""
            } else {
                notify({ content: Context.get(["dictionary", "transactionError"]), autoClose: true })
            }
        } catch (e) {
            notify({ content: e?.message || Context.get(["dictionary", "transactionError"]), autoClose: true })
        }

        this.$submit.removeAttribute("disabled")
        this.$amountIn.disabled = false
    }
}

customElements.define("route-swap", SWAP)

export default SWAP
