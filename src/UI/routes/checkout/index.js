import template from "./template.js"
import { render } from "/core/UI.js"

export class CHECKOUT extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }
}

customElements.define("route-checkout", CHECKOUT)

export default CHECKOUT
