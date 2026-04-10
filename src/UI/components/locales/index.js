import template from "./template.js"
import { Statics } from "/core/Stores.js"
import { Context } from "/core/Context.js"
import { render } from "/core/UI.js"
import Component from "/core/UI/Component.js"
import logic from "./logic.js"

export class LOCALES extends Component {
    static module = import.meta.url
    constructor() {
        super()
        this.template = template // Store for HMR
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }

    onconnect() {
        const button = this.shadowRoot.querySelector("ui-icon")
        const select = this.shadowRoot.querySelector("ui-picker")

        this.listen(button, "click", select.show)

        select.states.set({ options: logic.options(Statics.locales), selected: Context.get("locale")?.code })
        select.callback = (code) => Context.set({ locale: logic.find(code, Statics.locales) })
    }
}

customElements.define("ui-locales", LOCALES)

export default LOCALES
