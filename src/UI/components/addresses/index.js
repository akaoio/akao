import template from "./template.js"
import { render } from "/core/UI.js"
import { Context } from "/core/Context.js"

export class ADDRESSES extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
    }

    connectedCallback() {
        this.form = this.shadowRoot.querySelector("#address-form")
        this.form.querySelectorAll("input[type=\"text\"], input[type=\"email\"], input[type=\"tel\"]").forEach((input) => this.subscriptions.push(Context.on(["dictionary", input.name], [input, "placeholder"])))
    }
}

customElements.define("ui-addresses", ADDRESSES)