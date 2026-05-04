import template from "./template.js"
import { Context } from "/core/Context.js"
import { events } from "/core/Events.js"
import { Elements, Lives, Chains, Dexs, Wallets } from "/core/Stores.js"
import { notify, formatNumber } from "/core/Utils.js"
import Route from "/core/UI/Route.js"
import logic from "./logic.js"

export class SWAP extends Route {
    static module = import.meta.url
    constructor() {
        super(template)
        this.quote = this.quote.bind(this)
        this.submit = this.submit.bind(this)
    }

    onconnect() {
        this.$wallets = this.shadowRoot.querySelector("ui-wallets")
        this.$form = this.shadowRoot.querySelector("#swap-form")

        this.$form.onquote = this.quote
        this.$form.onswap = this.submit
        this.$form.onfrom = () => {
            this.$wallets.setCurrency(this.$form.from?.configs?.name || null)
            this._balance()
        }
        this.$form.onflip = () => {
            if (this.$form.from) this._balance()
            if (this.$form.from && this.$form.to && this.$form.amountIn) this.quote()
        }
        this.sub(
            this.$wallets.states.on(
                "address",
                ({ value }) => {
                    this.$form.enabled = !!value
                    if (value) this._loadTokens()
                },
                true
            ),
            this.$wallets.states.on("chain", () => this._loadTokens()),
            events.on("Lives.pools", () => {
                if (this.$form.from && this.$form.to && this.$form.amountIn) this.quote()
            })
        )

        // Pre-fill from URL params
        const params = Context.get("params") || {}
        if (params.from) this._pfrom = params.from
        if (params.to) this._pto = params.to

        Elements.Access?.checkpoint()
    }

    _loadTokens() {
        const chain = this.$wallets.states.get("chain")
        if (!chain) return
        const opts = logic.options(chain, Lives.pools?.[chain], Chains)
        this.$form.options = opts

        if (this._pfrom) {
            const found = opts.find((o) => o.configs?.symbol?.toLowerCase() === this._pfrom.toLowerCase())
            if (found) this.$form.selectToken("from", found.address)
            this._pfrom = null
        }
        if (this._pto) {
            const found = opts.find((o) => o.configs?.symbol?.toLowerCase() === this._pto.toLowerCase())
            if (found) this.$form.selectToken("to", found.address)
            this._pto = null
        }
    }

    _balance() {
        const chain = this.$wallets.states.get("chain")
        const balance = logic.balance(chain, this._resolve(this.$form.from), Lives.balances)
        if (balance === null) {
            this.$form.balanceIn = ""
            return
        }
        const label = Context.get(["dictionary", "balance"]) || "Balance"
        const symbol = this.$form.from?.configs?.name || ""
        this.$form.balanceIn = `${label}: ${formatNumber(balance)} ${symbol}`
    }

    _resolve(opt) {
        return opt || null
    }

    async _run() {
        this.$form.error = ""
        this.$form.quoteOut = "…"
        this.$form.gas = ""

        const chain = this.$wallets.states.get("chain")
        const pools = Lives.pools?.[chain]

        const result = await logic.quote({
            from: this._resolve(this.$form.from),
            to: this._resolve(this.$form.to),
            amount: this.$form.amountIn,
            chain,
            pools,
            Dexs,
            balances: Lives.balances,
            fiat: Context.get("fiat")?.code || "USD",
            forex: Lives.forex,
            Wallets,
            address: this.$wallets.states.get("address")
        })

        if (result.error) {
            const i18nKeys = ["insufficientBalance", "nopoolFound"]
            this.$form.error = i18nKeys.includes(result.error) ? Context.get(["dictionary", result.error]) || result.error : result.error
            this.$form.quoteOut = "0"
            this.$form.submitReady = false
            return
        }

        const { amountOut, gasAmount, gasSymbol } = result

        if (!amountOut) {
            this.$form.quoteOut = "0"
            this.$form.submitReady = false
            return
        }

        this.$form.quoteOut = `${formatNumber(amountOut)} ${this.$form.to?.configs?.name || ""}`
        this.$form.submitReady = true

        if (gasAmount !== null) {
            const label = Context.get(["dictionary", "gasFee"]) || "Gas fee"
            this.$form.gas = `${label}: ${gasAmount} ${gasSymbol}`
        }

        this._balance()
    }

    async submit() {
        const from = this.$form.from
        const to = this.$form.to
        const amount = this.$form.amountIn
        const slippage = this.$form.slippage
        const chain = this.$wallets.states.get("chain")

        this.$form.submitReady = false
        this.$form.enabled = false

        const result = await logic.swap({
            from: this._resolve(from),
            to: this._resolve(to),
            amount,
            slippage,
            chain,
            pools: Lives.pools?.[chain],
            Dexs,
            balances: Lives.balances
        })

        if (result.success) {
            notify({ content: Context.get(["dictionary", "transactionSent"]), autoClose: true })
            this.$form.clear()
        } else {
            const keys = ["missingRequiredFields", "insufficientBalance", "nopoolFound", "transactionError"]
            const msg = keys.includes(result.error) ? Context.get(["dictionary", result.error]) : result.error
            notify({ content: msg, autoClose: true })
        }

        this.$form.enabled = !!this.$wallets.states.get("address")
    }

    quote() {
        clearTimeout(this._qpend)
        this._qpend = setTimeout(() => this._run(), 500)
    }
}

customElements.define("route-swap", SWAP)

export default SWAP
