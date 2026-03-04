import template from "./template.js"
import { render } from "/core/UI.js"

export class WITHDRAW extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }
}

customElements.define("route-withdraw", WITHDRAW)

export default WITHDRAW
