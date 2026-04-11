import { Statics } from "/core/Stores.js"
import { Context } from "/core/Context.js"
import Component from "/core/UI/Component.js"
import logic from "./logic.js"

export class FIAT extends Component {
    static module = import.meta.url
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        this.render = this.render.bind(this)
    }

    static get observedAttributes() {
        return ["data-locale", "data-amount", "data-base", "data-quote"]
    }

    attributeChangedCallback(name, last, value) {
        if (["data-locale", "data-amount", "data-base", "data-quote"].includes(name) && last !== value) this.render()
    }

    onconnect() {
        this.sub(Context.on("locale", this.render), Context.on("fiat", this.render))
        this.render()
    }

    async render() {
        const locale = this.dataset.locale || Context.get("locale")?.code || Statics?.site?.locale
        const amount = Number(this.dataset.amount) || 0
        const base = this.dataset.base || Statics?.site?.fiat
        const quote = this.dataset.quote || Context.get("fiat")?.code || Statics?.site?.fiat
        const converted = await logic.convert(amount, base, quote)
        this.shadowRoot.innerHTML = new Intl.NumberFormat(locale, {
            style: "currency",
            currency: quote,
            currencyDisplay: "symbol"
        }).format(converted)
    }
}
customElements.define("ui-fiat", FIAT)

export default FIAT
