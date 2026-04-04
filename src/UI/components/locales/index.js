import template from "./template.js"
import { Statics } from "/core/Stores.js"
import { Context } from "/core/Context.js"
import { render } from "/core/UI.js"
import logic from "./logic.js"

export class LOCALES extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
    }

    connectedCallback() {
        const button = this.shadowRoot.querySelector("ui-icon")
        const select = this.shadowRoot.querySelector("ui-picker")

        button.addEventListener("click", select.show)
        this.subscriptions.push(() => button.removeEventListener("click", select.show))

        select.states.set({ options: logic.options(Statics.locales), selected: Context.get("locale")?.code })
        select.callback = (code) => Context.set({ locale: logic.find(code, Statics.locales) })
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }
}

customElements.define("ui-locales", LOCALES)

export default LOCALES
