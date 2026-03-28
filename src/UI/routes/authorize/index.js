import template from "./template.js"
import { render } from "/core/UI.js"
import { Elements } from "/core/Stores.js"

export class AUTHORIZE extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
    }

    connectedCallback() {
        Elements.Access?.checkpoint()
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }
}

customElements.define("route-authorize", AUTHORIZE)

export default AUTHORIZE