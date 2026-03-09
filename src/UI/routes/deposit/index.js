import template from "./template.js"
import { render } from "/core/UI.js"
import { Elements } from "/core/Stores.js"

export class DEPOSIT extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }

    connectedCallback() {
        const check = Elements.Access?.checkpoint()
        if (!check) return
    }
}

customElements.define("route-deposit", DEPOSIT)

export default DEPOSIT
