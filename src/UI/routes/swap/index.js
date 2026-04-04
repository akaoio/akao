import template from "./template.js"
import { render } from "/core/UI.js"
import { Context } from "/core/Context.js"
import { events } from "/core/Events.js"
import { Elements, Lives, Chains, Dexs, Wallets } from "/core/Stores.js"
import { notify, formatNumber } from "/core/Utils.js"
import logic from "./logic.js"

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

        this.$sfrom = (e) => {
            this.$from = e.detail
            this.quote()
            this.balance()
        }
        this.$sto = (e) => {
            this.$to = e.detail
            this.quote()
        }
        this.$fromToken.addEventListener("select", this.$sfrom)
        this.$toToken.addEventListener("select", this.$sto)

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
                    this.options()
                }
            }, true),
            this.$wallets.states.on("chain", () => this.options()),
            events.on("Lives.pools", () => this.options()),
            () => {
                this.$amountIn.removeEventListener("input", this.quote)
                this.$submit.removeEventListener("click", this.submit)
                this.$fromToken.removeEventListener("select", this.$sfrom)
                this.$toToken.removeEventListener("select", this.$sto)
            }
        )

        const params = Context.get("params") || {}
        if (params.from) this.$pfrom = params.from
        if (params.to) this.$pto = params.to

        Elements.Access?.checkpoint()
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }

    options() {
        const chain = this.$wallets.states.get("chain")
        if (!chain) return
        const opts = logic.options(chain, Lives.pools?.[chain], Chains)
        this.$fromToken.setoptions(opts)
        this.$toToken.setoptions(opts)
        if (this.$pfrom) {
            const found = opts.find((o) => o.address.toLowerCase() === this.$pfrom.toLowerCase())
            if (found) { this.$from = found; this.$fromToken.states.set({ selected: found }) }
            this.$pfrom = null
        }
        if (this.$pto) {
            const found = opts.find((o) => o.address.toLowerCase() === this.$pto.toLowerCase())
            if (found) { this.$to = found; this.$toToken.states.set({ selected: found }) }
            this.$pto = null
        }
    }

    balance() {
        const balance = logic.balance(this.$wallets.states.get("chain"), this.$from, Lives.balances)
        if (balance === null) { this.$balanceIn.textContent = ""; return }
        const label = Context.get(["dictionary", "balance"]) || "Balance"
        this.$balanceIn.textContent = `${label}: ${formatNumber(balance)} ${this.$from?.configs?.name || ""}`
    }

    async run() {
        this.$error.textContent = ""
        this.$quoteOut.textContent = ""
        this.$gas.textContent = ""

        const chain = this.$wallets.states.get("chain")
        const result = await logic.quote({
            from: this.$from,
            to: this.$to,
            amount: Number(this.$amountIn.value),
            chain,
            pools: Lives.pools?.[chain],
            Dexs,
            balances: Lives.balances,
            fiat: Context.get("fiat")?.code || "USD",
            forex: Lives.forex,
            Wallets,
            address: this.$wallets.states.get("address"),
        })

        if (result.error) {
            const i18nKeys = ["insufficientBalance", "nopoolFound"]
            this.$error.textContent = i18nKeys.includes(result.error)
                ? Context.get(["dictionary", result.error]) || result.error
                : result.error
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
        const chain = this.$wallets.states.get("chain")
        this.$submit.setAttribute("disabled", "")
        this.$amountIn.disabled = true

        const result = await logic.swap({
            from: this.$from,
            to: this.$to,
            amount: Number(this.$amountIn.value),
            slippage: Number(this.$slippage.value) || 0.5,
            chain,
            pools: Lives.pools?.[chain],
            Dexs,
            balances: Lives.balances,
        })

        if (result.success) {
            notify({ content: Context.get(["dictionary", "transactionSent"]), autoClose: true })
            this.$amountIn.value = ""
            this.$quoteOut.textContent = ""
            this.$gas.textContent = ""
        } else {
            const keys = ["missingRequiredFields", "insufficientBalance", "nopoolFound", "transactionError"]
            const msg = keys.includes(result.error) ? Context.get(["dictionary", result.error]) : result.error
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
