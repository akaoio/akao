import template from "./template.js"
import { render } from "/core/UI.js"
import BaseElement from "/UI/BaseElement.js"
import logic from "./logic.js"

export class HEADER extends BaseElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }

    onConnect() {
        this.listen(this.shadowRoot.querySelector(".games"), "click", () => {
            logic.open()
        })
    }
}

customElements.define("ui-header", HEADER)
