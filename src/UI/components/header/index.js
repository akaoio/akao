import template from "./template.js"
import { render } from "/core/UI.js"
import Component from "/core/UI/Component.js"
import logic from "./logic.js"

export class HEADER extends Component {
    static module = import.meta.url
    constructor() {
        super()
        this.template = template // Store for HMR
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }

    onconnect() {
        this.listen(this.shadowRoot.querySelector(".games"), "click", () => {
            logic.open()
        })
    }
}

customElements.define("ui-header", HEADER)
