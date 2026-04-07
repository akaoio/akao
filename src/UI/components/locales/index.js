import template from "./template.js"
import { Statics } from "/core/Stores.js"
import { Context } from "/core/Context.js"
import { render } from "/core/UI.js"
import BaseElement from "/UI/BaseElement.js"
import logic from "./logic.js"

export class LOCALES extends BaseElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }

    onConnect() {
        const button = this.shadowRoot.querySelector("ui-icon")
        const select = this.shadowRoot.querySelector("ui-picker")

        this.listen(button, "click", select.show)

        select.states.set({ options: logic.options(Statics.locales), selected: Context.get("locale")?.code })
        select.callback = (code) => Context.set({ locale: logic.find(code, Statics.locales) })
    }
}

customElements.define("ui-locales", LOCALES)

export default LOCALES
