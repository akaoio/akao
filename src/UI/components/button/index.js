import template from "./template.js"
import { render } from "/core/UI.js"

export class BUTTON extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }

    connectedCallback() {
        this.shadowRoot.querySelector("button").classList = this.classList
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
}

customElements.define("ui-button", BUTTON)
