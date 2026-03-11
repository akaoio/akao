import template from "./template.js"
import { Access, setWallet } from "/core/Access.js"
import { html, render } from "/core/UI.js"
import { Chains, Wallets } from "/core/Stores.js"
import States from "/core/States.js"
import { Context } from "/core/Context.js"
import SELECT from "/UI/components/select/index.js"

export class WALLETS extends HTMLElement {
    constructor() {
        super()
        this.states = new States({
            chain: Context.get("params")?.chain || null,
            currency: Context.get("params")?.currency || null,
            address: null
        })
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
        this.step = 5
        this.increase = this.increase.bind(this)
        this.decrease = this.decrease.bind(this)
        this.create = this.create.bind(this)
        this.remove = this.remove.bind(this)
        this.select = this.select.bind(this)
        this.change = this.change.bind(this)
        this.render = this.render.bind(this)
    }

    get id() {
        return Number(Access.get("wallet")?.id || 0)
    }

    set id(value) {
        value = Number(value)
        const total = value >= this.total ? Math.ceil((value + 1) / this.step) * this.step : this.total
        setWallet({ id: value, total: total !== this.total ? total : undefined })
        return value
    }

    get total() {
        return Number(typeof this.dataset.total !== "undefined" ? this.dataset.total : Access.get("wallet")?.total || this.step)
    }

    set total(value) {
        return setWallet({ total: Number(value) })
    }

    async connectedCallback() {
        this.$wallets = this.shadowRoot.querySelector("#wallets")
        this.shadowRoot.querySelector("#increase").addEventListener("click", this.increase)
        this.shadowRoot.querySelector("#decrease").addEventListener("click", this.decrease)
        this.subscriptions.push(
            Access.on("authenticated", ({ value }) => {
                this.toggleAttribute("hidden", !value)
                if (value) this.change()
                else {
                    this.states.set({ address: null })
                    while (this.$wallets.firstChild) this.$wallets.removeChild(this.$wallets.firstChild)
                }
            }),
            Access.on("wallet", this.change),
            this.states.on("chain", this.render),
            this.states.on("currency", this.render),
            () => this.shadowRoot.querySelector("#increase").removeEventListener("click", this.increase),
            () => this.shadowRoot.querySelector("#decrease").removeEventListener("click", this.decrease)
        )

        this.toggleAttribute("hidden", !Access.get("authenticated"))

        // Load currency contracts for all chains
        for (const chain of Object.values(Chains)) {
            if (!chain.configs?.currencies?.length) return
            if (chain.configs?.currencies?.length !== Object.values(chain?.currencies)?.length) await chain.load()
        }

        const currency = this.states.get("currency") || this.currencies[0]?.value || null
        const chains = this.chains(currency)
        const chain = chains.some(o => o.value === this.states.get("chain"))
            ? this.states.get("chain")
            : chains[0]?.value || null
        this.states.set({ currency: currency, chain: chain })

        this.$chains = new SELECT({
            name: "chain",
            options: chains,
            placeholder: "dictionary.chain",
            selected: chain,
            change: this.change
        })
        render(this.$chains, this.shadowRoot.querySelector("#chains"), { append: true })

        this.$currencies = new SELECT({
            name: "currency",
            options: this.currencies,
            placeholder: "dictionary.currency",
            selected: currency,
            change: this.change
        })
        render(this.$currencies, this.shadowRoot.querySelector("#currencies"), { append: true })

        this.$address = this.shadowRoot.querySelector("#address")
        this.$balance = this.shadowRoot.querySelector("#balance")

        if (Access.get("authenticated")) this.change()
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }

    increase() {
        this.total += this.step
    }

    decrease() {
        if (this.total - this.step > this.id) this.total -= this.step
    }

    async create() {
        if (this.$wallets.children.length >= this.total) return
        const templates = []
        for (let id = this.$wallets.children.length; id < this.total; id++) {
            const seed = await globalThis.sea.work(Access.get("seed"), id)
            const select = () => this.select({ id })
            templates.push(html`
                <span class="wallet">
                    <input id="i${id}" type="radio" name="wallet" value="${id}" ${id === this.id ? "checked" : ""} />
                    <label
                        for="i${id}"
                        ${({ element }) => {
                            element.addEventListener("click", select)
                            this.subscriptions.push(() => element.removeEventListener("click", select))
                        }}>
                        <ui-identicon data-size="5" data-seed="${seed}" />
                    </label>
                </span>
            `)
        }
        render(templates, this.$wallets, { append: true })
    }

    remove() {
        const count = this.$wallets.children.length
        const min = Math.max(this.step, this.id + 1)
        if (count <= min) return
        for (let i = 0; i < Math.min(this.step, count - this.total); i++) this.$wallets.removeChild(this.$wallets.lastChild)
        if (this.$wallets.children.length > this.total) this.remove()
    }

    async select({ id }) {
        if (!this.$wallets.querySelector(`input#i${id}`)) await this.create()
        this.id = id
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
            } else {
                this.states.set({ [name]: value })
            }
        }
        this.render()
        if (this.states.get("chain") && this.states.get("currency")) {
            const wallet = Wallets[this.states.get("chain")]
            const address = wallet?.address || null
            this.states.set({ address })
            this.$address.textContent = address || ""
            const currency = Object.values(wallet.chain.currencies).find((c) => c.name === this.states.get("currency"))
            const balance = await wallet.balance({ currency })
            if (typeof balance !== undefined) this.$balance.textContent = balance
        }
    }

    get currencies() {
        const currencies = {}
        for (const chain of Object.values(Chains)) {
            for (const currency of Object.values(chain.currencies)) {
                if (!currency?.name) continue
                currencies[currency.name] = {
                    name: currency.name,
                    symbol: currency.symbol
                }
            }
        }
        return Object.values(currencies)
            .sort((a, b) => a.name.localeCompare(b.name))
            .map((currency) => ({
                label: html`<ui-svg class="icon" data-src="/images/cryptos/${currency.symbol}" /> ${currency.name}`,
                value: currency.name
            }))
    }

    chains(currency = null) {
        return Object.values(Chains)
            .filter(chain => !currency || Object.values(chain.currencies).some(c => c.name === currency))
            .map((chain) => ({
                label: html`<ui-svg class="icon" data-src="/images/cryptos/${chain.configs.symbol}" /> ${chain.configs.name || String(chain.id)}`,
                value: String(chain.id)
            }))
    }

    render() {
        if (this._renderPending) return
        this._renderPending = true
        queueMicrotask(async () => {
            this._renderPending = false
            if (!Access.get("authenticated")) return
            if (this.$wallets.children.length < this.total) await this.create()
            if (this.$wallets.children.length > this.total) this.remove()
        })
    }
}

customElements.define("ui-wallets", WALLETS)
export default WALLETS
