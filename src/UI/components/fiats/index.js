import template from "./template.js"
import { Statics } from "/core/Stores.js"
import { Context, setFiat } from "/core/Context.js"
import { render } from "/core/UI.js"
import logic from "./logic.js"

export class FIATS extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }

    connectedCallback() {
        const span = this.shadowRoot.querySelector("span")
        const select = this.shadowRoot.querySelector("ui-picker")

        // Set initial fiat symbol
        this.subscription = Context.on(["fiat", "symbol"], [span, "innerText"])

        span.addEventListener("click", () => select.show())

        select.states.set({ options: logic.options(Statics.fiats), selected: Context.get("fiat")?.code })
        select.callback = setFiat
    }

    disconnectedCallback() {
        this.subscription.off()
    }
}

customElements.define("ui-fiats", FIATS)

export default FIATS
