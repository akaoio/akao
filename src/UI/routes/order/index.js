import template from "./template.js"
import { render } from "/core/UI.js"

export class ORDER extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }
}

customElements.define("route-order", ORDER)

export default ORDER
