import { Statics } from "/core/Stores.js"
import { Context } from "/core/Context.js"
import Forex from "/core/Forex.js"

export class FIAT extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        this.subscriptions = []
        this.render = this.render.bind(this)
    }

    static get observedAttributes() {
        return ["data-locale", "data-amount", "data-base", "data-quote"]
    }

    attributeChangedCallback(name, old, value) {
        if (["data-locale", "data-amount", "data-base", "data-quote"].includes(name) && old !== value) this.render()
    }

    connectedCallback() {
        this.subscriptions.push(
            Context.on("locale", this.render),
            Context.on("fiat", this.render)
        )
    }

    disconnected() {
        this.subscriptions.forEach((off) => off())
    }

    async render() {
        const locale = this.dataset.locale || Context.get("locale")?.code || Statics?.site?.locale
        const amount = Number(this.dataset.amount) || 0
        const base = this.dataset.base || Statics?.site?.fiat
        const quote = this.dataset.quote || Context.get("fiat")?.code || Statics?.site?.fiat
        const converted = await Forex.convert(amount, base, quote)
        this.shadowRoot.innerHTML = new Intl.NumberFormat(locale, {
            style: "currency",
            currency: quote
        }).format(converted)
    }
}
customElements.define("ui-fiat", FIAT)

export default FIAT