import template from "./template.js"
import { html, render } from "/core/UI.js"
import { Context } from "/core/Context.js"
import { events } from "/core/Events.js"
import { Elements, Lives, Chains, Dexs, Wallets } from "/core/Stores.js"
import { notify, formatNumber } from "/core/Utils.js"
import Route from "/core/UI/Route.js"
import SELECT from "/UI/components/select/index.js"
import logic from "./logic.js"

export class SWAP extends Route {
    static module = import.meta.url
    constructor() {
        super(template)
        this.quote = this.quote.bind(this)
        this.submit = this.submit.bind(this)
        this.flip = this.flip.bind(this)
    }

    onconnect() {
        this.$wallets = this.shadowRoot.querySelector("ui-wallets")
        this.$amountIn = this.shadowRoot.querySelector("#amount-in")
        this.$quoteOut = this.shadowRoot.querySelector("#quote-out")
        this.$balanceIn = this.shadowRoot.querySelector("#balance-in")
        this.$slippage = this.shadowRoot.querySelector("#slippage")
        this.$gas = this.shadowRoot.querySelector("#gas")
        this.$error = this.shadowRoot.querySelector("#error")
        this.$submit = this.shadowRoot.querySelector("#submit")
        this.$flipBtn = this.shadowRoot.querySelector("#flip-btn")

        // From token selector
        this.$fromToken = new SELECT({
            name: "from-token",
            options: [],
            placeholder: "dictionary.selectToken",
            change: (e) => {
                const opt = this.tokenMap.get(e.target.value)
                if (opt) {
                    this.$from = opt
                    this.quote()
                    this.balance()
                }
            }
        })
        render(this.$fromToken, this.shadowRoot.querySelector("#from-token"), { append: true })

        // To token selector
        this.$toToken = new SELECT({
            name: "to-token",
            options: [],
            placeholder: "dictionary.selectToken",
            change: (e) => {
                const opt = this.tokenMap.get(e.target.value)
                if (opt) {
                    this.$to = opt
                    this.quote()
                }
            }
        })
        render(this.$toToken, this.shadowRoot.querySelector("#to-token"), { append: true })

        // Wire events
        this.listen(this.$amountIn, "input", this.quote)
        this.listen(this.$submit, "click", this.submit)
        this.listen(this.$flipBtn, "click", this.flip)

        // Slippage presets
        this.shadowRoot.querySelectorAll(".slippage-preset").forEach((btn) => {
            this.listen(btn, "click", () => {
                this.$slippage.value = btn.dataset.value
                this.shadowRoot.querySelectorAll(".slippage-preset").forEach(b => b.classList.remove("active"))
                btn.classList.add("active")
            })
        })
        const defaultPreset = this.shadowRoot.querySelector(`.slippage-preset[data-value="0.5"]`)
        if (defaultPreset) defaultPreset.classList.add("active")

        this.sub(
            this.$wallets.states.on("address", ({ value }) => {
                const active = !!value
                this.$amountIn.disabled = !active
                this.$submit.toggleAttribute("disabled", !active)
                if (!active) {
                    this.$quoteOut.textContent = ""
                    this.$gas.textContent = ""
                    this.$error.textContent = ""
                } else this.options()
            }, true),
            this.$wallets.states.on("chain", () => this.options()),
            events.on("Lives.pools", () => this.options())
        )

        // Pre-fill from URL params
        const params = Context.get("params") || {}
        if (params.from) this.$pfrom = params.from
        if (params.to) this.$pto = params.to

        Elements.Access?.checkpoint()
    }

    options() {
        const chain = this.$wallets.states.get("chain")
        if (!chain) return
        const opts = logic.options(chain, Lives.pools?.[chain], Chains)

        this.tokenMap = new Map()
        const selectOpts = opts.map((opt) => {
            this.tokenMap.set(opt.address, opt)
            return {
                label: html`
                    <ui-svg class="icon" data-src="/images/cryptos/${opt.configs?.symbol || ""}" />
                    ${opt.configs?.name || opt.address}
                `,
                value: opt.address
            }
        })

        this.$fromToken.states.set({ options: selectOpts })
        this.$toToken.states.set({ options: selectOpts })

        if (this.$pfrom) {
            const found = opts.find((o) => o.address.toLowerCase() === this.$pfrom.toLowerCase())
            if (found) {
                this.$from = found
                this.$fromToken.states.set({ selected: found.address })
            }
            this.$pfrom = null
        }
        if (this.$pto) {
            const found = opts.find((o) => o.address.toLowerCase() === this.$pto.toLowerCase())
            if (found) {
                this.$to = found
                this.$toToken.states.set({ selected: found.address })
            }
            this.$pto = null
        }

        // Re-run quote if both tokens still valid after chain switch
        if (this.$from && this.$to) this.quote()
        else {
            this.$from = null
            this.$to = null
            this.$quoteOut.textContent = "0"
            this.$gas.textContent = ""
            this.$error.textContent = ""
        }
    }

    balance() {
        const balance = logic.balance(this.$wallets.states.get("chain"), this.$from, Lives.balances)
        if (balance === null) {
            this.$balanceIn.textContent = ""
            return
        }
        const label = Context.get(["dictionary", "balance"]) || "Balance"
        this.$balanceIn.textContent = `${label}: ${formatNumber(balance)} ${this.$from?.configs?.name || ""}`
    }

    flip() {
        const prevFrom = this.$from
        const prevTo = this.$to

        this.$from = prevTo
        this.$to = prevFrom

        if (this.$from) this.$fromToken.states.set({ selected: this.$from.address })
        if (this.$to) this.$toToken.states.set({ selected: this.$to.address })

        // Move amount to sell side, clear buy side
        this.$amountIn.value = ""
        this.$quoteOut.textContent = "0"
        this.$gas.textContent = ""
        this.$error.textContent = ""
        this.$balanceIn.textContent = ""

        if (this.$from) this.balance()
        if (this.$from && this.$to && this.$amountIn.value) this.quote()
    }

async run() {
        this.$error.textContent = ""
        this.$quoteOut.textContent = "…"
        this.$gas.textContent = ""

        const result = await logic.quote({
            from: this.$from,
            to: this.$to,
            amount: Number(this.$amountIn.value),
            chain: this.$wallets.states.get("chain"),
            pools: Lives.pools?.[this.$wallets.states.get("chain")],
            Dexs,
            balances: Lives.balances,
            fiat: Context.get("fiat")?.code || "USD",
            forex: Lives.forex,
            Wallets,
            address: this.$wallets.states.get("address")
        })

        if (result.error) {
            const i18nKeys = ["insufficientBalance", "nopoolFound"]
            this.$error.textContent = i18nKeys.includes(result.error)
                ? Context.get(["dictionary", result.error]) || result.error
                : result.error
            this.$quoteOut.textContent = "0"
            this.$submit.setAttribute("disabled", "")
            return
        }

        const { amountOut, fiatOut, gasAmount, gasSymbol } = result
        const fiat = Context.get("fiat")?.code || "USD"
        const locale = Context.get("locale")?.code || "en"

        let quoteText = `${formatNumber(amountOut)} ${this.$to?.configs?.name || ""}`
        if (fiatOut > 0) quoteText += " ≈ " + new Intl.NumberFormat(locale, { style: "currency", currency: fiat, notation: "compact" }).format(fiatOut)
        this.$quoteOut.textContent = quoteText

        if (gasAmount !== null) {
            const label = Context.get(["dictionary", "gasFee"]) || "Gas fee"
            this.$gas.textContent = `${label}: ${gasAmount} ${gasSymbol}`
        }

        this.$submit.removeAttribute("disabled")
    }

    async submit() {
        this.$submit.setAttribute("disabled", "")
        this.$amountIn.disabled = true

        const result = await logic.swap({
            from: this.$from,
            to: this.$to,
            amount: Number(this.$amountIn.value),
            slippage: Number(this.$slippage.value) || 0.5,
            chain: this.$wallets.states.get("chain"),
            pools: Lives.pools?.[this.$wallets.states.get("chain")],
            Dexs,
            balances: Lives.balances
        })

        if (result.success) {
            notify({ content: Context.get(["dictionary", "transactionSent"]), autoClose: true })
            this.$amountIn.value = ""
            this.$quoteOut.textContent = "0"
            this.$gas.textContent = ""
        } else {
            const keys = ["missingRequiredFields", "insufficientBalance", "nopoolFound", "transactionError"]
            const msg = keys.includes(result.error)
                ? Context.get(["dictionary", result.error])
                : result.error
            notify({ content: msg, autoClose: true })
        }

        this.$submit.removeAttribute("disabled")
        this.$amountIn.disabled = false
    }

    quote() {
        clearTimeout(this.$qpend)
        this.$qpend = setTimeout(() => this.run(), 500)
    }
}

customElements.define("route-swap", SWAP)

export default SWAP
