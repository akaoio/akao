import template from "./template.js"
import { render } from "/core/UI.js"
import Component from "/core/UI/Component.js"

export class BUTTON extends Component {
    static module = import.meta.url
    static observedAttributes = ["disabled"]

    constructor() {
        super()
        this.template = template // Store for HMR
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }

    onconnect() {
        const btn = this.shadowRoot.querySelector("button")
        btn.classList = this.classList
        btn.disabled = this.hasAttribute("disabled")
        let left = this.dataset.left
        let right = this.dataset.right
        if (left) {
            if (!left.includes("/")) left = `/images/icons/${left}.svg`
            this.shadowRoot.querySelector("#left").dataset.src = left
        }
        if (right) {
            if (!right.includes("/")) right = `/images/icons/${right}.svg`
            this.shadowRoot.querySelector("#right").dataset.src = right
        }
    }

    attributeChangedCallback(name, _old, value) {
        if (name === "disabled") {
            const btn = this.shadowRoot?.querySelector("button")
            if (btn) btn.disabled = value !== null
        }
    }
}

customElements.define("ui-button", BUTTON)
