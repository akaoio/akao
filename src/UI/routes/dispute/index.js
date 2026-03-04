import template from "./template.js"
import { render } from "/core/UI.js"

export class DISPUTE extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }
}

customElements.define("route-dispute", DISPUTE)

export default DISPUTE
