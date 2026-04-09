import template from "./template.js"
import { Access } from "/core/Access.js"
import { html, render } from "/core/UI.js"
import { Chains, Lives, Wallets } from "/core/Stores.js"
import States from "/core/States.js"
import { Context } from "/core/Context.js"
import SELECT from "/UI/components/select/index.js"
import logic from "./logic.js"

export class WALLETS extends HTMLElement {
    static module = import.meta.url
    constructor() {
        super()
        this.template = template // Store for HMR
        this.states = new States({
            chain: Context.get("params")?.chain || null,
            currency: Context.get("params")?.currency || null,
            address: null
        })
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subs = []
        this.step = 5
        this.change = this.change.bind(this)
    }

    get id() {
        return logic.id()
    }

    set id(value) {
        return logic.setid(value, this.step, this.total)
    }

    get total() {
        return Number(typeof this.dataset.total !== "undefined" ? this.dataset.total : logic.total() || this.step)
    }

    set total(value) {
        return logic.settotal(value)
    }

    async connectedCallback() {
        this.$identicons = this.shadowRoot.querySelector("ui-identicons")

        const seed = async () => {
            const s = await logic.seed()
            if (s != null) this.$identicons.dataset.seed = s
        }

        this.subs.push(
            this.$identicons.events.on("select", ({ detail: { id } }) => { this.id = id }),
            this.$identicons.events.on("increase", () => { this.total += this.step }),
            this.$identicons.events.on("decrease", () => { if (this.total - this.step > this.id) this.total -= this.step }),
            Access.on("authenticated", async ({ value }) => {
                this.toggleAttribute("hidden", !value)
                if (value) { await seed(); this.change() }
                else {
                    this.states.set({ address: null })
                    this.$identicons.clear()
                }
            }),
            Access.on("wallet", this.change)
        )

        this.toggleAttribute("hidden", !Access.get("authenticated"))

        // Load currency contracts for all chains
        for (const chain of Object.values(Chains)) {
            if (!chain.configs?.currencies?.length) return
            if (chain.configs?.currencies?.length !== Object.values(chain?.currencies)?.length) await chain.load()
        }

        let currency, chains, chain
        
        if (this.dataset.currency === "false") {
            // For swap route: no currency selector, show all chains
            chains = Object.values(Chains).map(c => ({
                label: html`<ui-svg class="icon" data-src="/images/cryptos/${c.configs?.symbol || ""}" /> ${c.configs?.name || c.id}`,
                value: c.id
            }))
            chain = this.states.get("chain") || chains[0]?.value || null
            this.states.set({ chain })
        } else {
            // For deposit/withdraw: show currency + chains that support it
            currency = this.states.get("currency") || this.currencies[0]?.value || null
            chains = this.chains(currency)
            chain = chains.some(o => o.value === this.states.get("chain"))
                ? this.states.get("chain")
                : chains[0]?.value || null
            this.states.set({ currency: currency, chain: chain })
        }

        this.$chains = new SELECT({
            name: "chain",
            options: chains,
            placeholder: "dictionary.chain",
            selected: chain,
            change: this.change
        })
        render(this.$chains, this.shadowRoot.querySelector("#chains"), { append: true })

        if (this.dataset.currency !== "false") {
            this.$currencies = new SELECT({
                name: "currency",
                options: this.currencies,
                placeholder: "dictionary.currency",
                selected: currency,
                change: this.change
            })
            render(this.$currencies, this.shadowRoot.querySelector("#currencies"), { append: true })
        }

        this.$address = this.shadowRoot.querySelector("#address")
        this.$balance = this.shadowRoot.querySelector("#balance")

        if (Access.get("authenticated")) seed().then(() => this.change())
    }

    disconnectedCallback() {
        this.subs.forEach((off) => off())
    }

    async change(event) {
        if (!Access.get("authenticated")) return
        if (event?.target) {
            const { name, value } = event.target
            if (name === "currency") {
                const options = this.chains(value)
                const chain = options.some(o => o.value === this.states.get("chain"))
                    ? this.states.get("chain")
                    : options[0]?.value || null
                this.$chains.states.set({ options, selected: chain })
                this.states.set({ currency: value, chain })
            } else 
                this.states.set({ [name]: value })
            
        }
        this.$identicons.id = this.id
        this.$identicons.total = this.total
        
        const chain = this.states.get("chain")
        const wallet = Wallets[chain]
        const address = wallet?.address || null
        this.states.set({ address })
        this.$address.textContent = address || ""
        
        if (this.dataset.currency === "false") return
        
        if (chain && this.states.get("currency")) {
            const currency = logic.currency(wallet, this.states.get("currency"))
            const fiat = Context.get("fiat")?.code || "USD"
            const { raw, amount } = await logic.balance({ wallet, currency, fiat, forex: Lives.forex })
            if (raw !== null) {
                const locale = Context.get("locale")?.code || "en"
                const fiatstr = amount > 0
                    ? " ≈ " + new Intl.NumberFormat(locale, { style: "currency", currency: fiat, notation: "compact" }).format(amount)
                    : ""
                this.$balance.textContent = `${raw}${fiatstr}`
            }
        }
    }

    get currencies() {
        return logic.currencies(Chains).map(c => ({
            label: html`<ui-svg class="icon" data-src="/images/cryptos/${c.symbol}" /> ${c.name}`,
            value: c.name
        }))
    }

    chains(currency = null) {
        return logic.chains(Chains, currency).map(c => ({
            label: html`<ui-svg class="icon" data-src="/images/cryptos/${c.symbol}" /> ${c.name}`,
            value: c.id
        }))
    }
}

customElements.define("ui-wallets", WALLETS)
export default WALLETS
