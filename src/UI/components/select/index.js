import { render } from "/core/UI.js"
import { template } from "./template.js"

export class SELECT extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }

    static get observedAttributes() {
        return ["data-name", "data-placeholder", "data-required"]
    }

    attributeChangedCallback(name, last, value) {
        if (last === value) return
        if (name === "data-name") this.select.setAttribute("name", value)
        else if (name === "data-placeholder") this.placeholder.dataset.key = value
        else if (name === "data-required") {
            if (value !== null) this.select.setAttribute("required", "")
            else this.select.removeAttribute("required")
        }
    }

    connectedCallback() {
        this.select = this.shadowRoot.querySelector("select")
        this.placeholder = this.shadowRoot.querySelector("#placeholder")
        if (this.dataset.required) this.select.setAttribute("required", "")
        if (this.dataset.name) this.select.setAttribute("name", this.dataset.name)
        if (this.dataset.placeholder) this.placeholder.dataset.key = this.dataset.placeholder
    }
}

customElements.define("ui-select", SELECT)

export default SELECT