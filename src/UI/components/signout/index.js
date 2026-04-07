import { Elements } from "/core/Stores.js"
import template from "./template.js"
import { render } from "/core/UI.js"
import BaseElement from "/UI/BaseElement.js"
import logic from "./logic.js"

export class SIGNOUT extends BaseElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.toggle = this.toggle.bind(this)
        this.signout = this.signout.bind(this)
    }

    onConnect() {
        this.listen(this.shadowRoot.querySelector("#signout"), "click", this.toggle)
        this.listen(this.shadowRoot.querySelector("#confirm"), "click", this.signout)
        this.listen(this.shadowRoot.querySelector("#back"), "click", this.toggle)
    }

    signout() {
        logic.signout()
        this.shadowRoot.querySelector("ui-modal").close()
    }

    toggle() {
        const check = Elements.Access?.checkpoint()
        if (!check) return
        this.shadowRoot.querySelector("ui-modal").toggleModal()
    }
}

customElements.define("ui-signout", SIGNOUT)

export default SIGNOUT
