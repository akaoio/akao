import template from "./template.js"
import { render } from "/core/UI.js"

export class DEPOSIT extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }
}

customElements.define("route-deposit", DEPOSIT)

export default DEPOSIT
