import template from "./template.js"
import { render } from "/core/UI.js"
import logic from "./logic.js"

export class HEADER extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }

    connectedCallback() {
        this.shadowRoot.querySelector(".games")?.addEventListener("click", () => {
            logic.open()
        })
    }
}

customElements.define("ui-header", HEADER)
