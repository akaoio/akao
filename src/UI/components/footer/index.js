import template from "./template.js"
import { render } from "/core/UI.js"

export class FOOTER extends HTMLElement {
    static module = import.meta.url
    constructor() {
        super()
        this.template = template // Store for HMR
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }
}

customElements.define("ui-footer", FOOTER)
