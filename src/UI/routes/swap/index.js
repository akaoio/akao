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
        this.quote  = this.quote.bind(this)
        this.submit = this.submit.bind(this)
    }

    onconnect() {
        this.$wallets = this.shadowRoot.querySelector("ui-wallets")
        this.$form    = this.shadowRoot.querySelector("#swap-form")

        this.$form.onquote = this.quote
        this.$form.onswap  = this.submit
        this.$form.onflip  = () => {
            if (this.$form.from) this._balance()
            if (this.$form.from && this.$form.to && this.$form.amountIn) this.quote()
        }

        this.sub(
            this.$wallets.states.on("address", ({ value }) => {
                this.$form.enabled = !!value
                if (value) this.options()
            }, true),
            this.$wallets.states.on("chain", () => this.options()),
            events.on("Lives.pools", () => this.options())
        )

        // Pre-fill from URL params
        const params = Context.get("params") || {}
        if (params.from) this._pfrom = params.from
        if (params.to)   this._pto   = params.to

        Elements.Access?.checkpoint()
    }

    options() {
        const chain = this.$wallets.states.get("chain")
        if (!chain) return
        const opts = logic.options(chain, Lives.pools?.[chain], Chains)

        this.$form.options = opts

        if (this._pfrom) {
            const found = opts.find(o => o.address.toLowerCase() === this._pfrom.toLowerCase())
            if (found) this.$form.selectToken("from", found.address)
            this._pfrom = null
        }
        if (this._pto) {
            const found = opts.find(o => o.address.toLowerCase() === this._pto.toLowerCase())
            if (found) this.$form.selectToken("to", found.address)
            this._pto = null
        }

        const poolCount = chain && Lives.pools?.[chain] ? Object.keys(Lives.pools[chain]).length : null
        this.$form.updateToolbar({ chain, poolCount })

        if (this.$form.from && this.$form.to) this.quote()
        else {
            this.$form.quoteOut = "0"
            this.$form.gas      = ""
            this.$form.error    = ""
        }
    }

    _balance() {
        const balance = logic.balance(this.$wallets.states.get("chain"), this.$form.from, Lives.balances)
        if (balance === null) { this.$form.balanceIn = ""; return }
        const label  = Context.get(["dictionary", "balance"]) || "Balance"
        const symbol = this.$form.from?.configs?.name || ""
        this.$form.balanceIn = `${label}: ${formatNumber(balance)} ${symbol}`
    }

    async _run() {
        this.$form.error    = ""
        this.$form.quoteOut = "…"
        this.$form.gas      = ""

        const chain = this.$wallets.states.get("chain")
        const pools = Lives.pools?.[chain]

        const result = await logic.quote({
            from:     this.$form.from,
            to:       this.$form.to,
            amount:   this.$form.amountIn,
            chain,
            pools,
            Dexs,
            balances: Lives.balances,
            fiat:     Context.get("fiat")?.code || "USD",
            forex:    Lives.forex,
            Wallets,
            address:  this.$wallets.states.get("address")
        })

        if (result.error) {
            const i18nKeys = ["insufficientBalance", "nopoolFound"]
            this.$form.error       = i18nKeys.includes(result.error)
                ? Context.get(["dictionary", result.error]) || result.error
                : result.error
            this.$form.quoteOut    = "0"
            this.$form.submitReady = false
            const poolCount = chain && Lives.pools?.[chain] ? Object.keys(Lives.pools[chain]).length : null
            this.$form.updateToolbar({ chain, poolCount })
            return
        }

        const found = logic.find(this.$form.from?.address, this.$form.to?.address, chain, pools, Dexs)
        if (found) {
            const poolCount = chain && Lives.pools?.[chain] ? Object.keys(Lives.pools[chain]).length : null
            this.$form.updateToolbar({
                chain,
                chainName: Chains[chain]?.name || chain,
                dexLabel:  `${found.dex.name || found.pool.dex} ${found.pool.version}`,
                fee:       found.pool.fee,
                poolCount,
            })
        }

        const { amountOut, gasAmount, gasSymbol } = result

        if (!amountOut) {
            this.$form.quoteOut    = "0"
            this.$form.submitReady = false
            return
        }

        this.$form.quoteOut    = `${formatNumber(amountOut)} ${this.$form.to?.configs?.name || ""}`
        this.$form.submitReady = true

        if (gasAmount !== null) {
            const label = Context.get(["dictionary", "gasFee"]) || "Gas fee"
            this.$form.gas = `${label}: ${gasAmount} ${gasSymbol}`
        }

        this._balance()
    }

    async submit() {
        const from     = this.$form.from
        const to       = this.$form.to
        const amount   = this.$form.amountIn
        const slippage = this.$form.slippage
        const chain    = this.$wallets.states.get("chain")

        this.$form.submitReady = false
        this.$form.enabled     = false

        const result = await logic.swap({
            from,
            to,
            amount,
            slippage,
            chain,
            pools:    Lives.pools?.[chain],
            Dexs,
            balances: Lives.balances
        })

        if (result.success) {
            notify({ content: Context.get(["dictionary", "transactionSent"]), autoClose: true })
            this.$form.clear()
        } else {
            const keys = ["missingRequiredFields", "insufficientBalance", "nopoolFound", "transactionError"]
            const msg = keys.includes(result.error)
                ? Context.get(["dictionary", result.error])
                : result.error
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
