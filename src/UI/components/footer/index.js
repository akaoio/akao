import template from "./template.js"
import { render } from "/core/UI.js"
import { Elements } from "/core/Stores.js"
import { Access } from "/core/Access.js"

export class FOOTER extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.onWaveAuth = this.onWaveAuth.bind(this)
    }

    connectedCallback() {
        this.shadowRoot.querySelector("#wave-auth")?.addEventListener("click", this.onWaveAuth)
    }

    disconnectedCallback() {
        this.shadowRoot.querySelector("#wave-auth")?.removeEventListener("click", this.onWaveAuth)
    }

    onWaveAuth() {
        if (Access.get("authenticated")) Elements.Access?.openWaveShare?.()
        else Elements.Access?.openWaveSignin?.()
    }
}

customElements.define("ui-footer", FOOTER)
