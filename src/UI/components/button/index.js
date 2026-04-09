import template from "./template.js"
import { render } from "/core/UI.js"
import Component from "/core/UI/Component.js"

export class BUTTON extends Component {
    static module = import.meta.url
    constructor() {
        super()
        this.template = template // Store for HMR
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }

    onconnect() {
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
