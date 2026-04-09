import { render } from "/core/UI.js"
import template from "./template.js"

export class CRYPTO extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }

    static get observedAttributes() {
        return ["data-symbol", "data-name", "data-amount", "data-fiat"]
    }

    attributeChangedCallback(name, last, value) {
        if (last === value) return
        if (name === "data-symbol") {
            const el = this.shadowRoot.querySelector("ui-svg")
            if (el) el.dataset.src = `/images/cryptos/${value}`
        } else if (name === "data-name") {
            const el = this.shadowRoot.querySelector(".name")
            if (el) el.textContent = value || ""
        } else if (name === "data-amount") {
            const el = this.shadowRoot.querySelector(".amount")
            if (el) el.textContent = value || ""
        } else if (name === "data-fiat") {
            const el = this.shadowRoot.querySelector(".fiat")
            if (el) el.textContent = value || ""
        }
    }
}

customElements.define("ui-pool-crypto", CRYPTO)

export default CRYPTO
