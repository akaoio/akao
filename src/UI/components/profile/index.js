import { Elements } from "/core/Stores.js"
import { Access } from "/core/Access.js"
import { render } from "/core/UI.js"
import template from "./template.js"

export class PROFILE extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        Elements.Profile = this
        render(template, this.shadowRoot)
        this.subscriptions = []
        this.toggle = this.toggle.bind(this)
    }

    get modal() {
        return this.shadowRoot.querySelector("ui-modal")
    }

    connectedCallback() {
        this.subscriptions.push(Access.on("authenticated", ({ value }) => value === false && this.modal.close()))
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }

    toggle() {
        const check = Elements.Access?.checkpoint()
        if (!check) return
        this.modal.toggleModal()
    }
}

customElements.define("ui-profile", PROFILE)

export default PROFILE