import template from "./template.js"
import { render } from "/core/UI.js"

export class INVENTORY extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }
}

customElements.define("route-inventory", INVENTORY)

export default INVENTORY
