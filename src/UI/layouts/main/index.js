import template from "./template.js"
import { render } from "/core/UI.js"

export class MAIN extends HTMLElement {
    constructor(props = {}) {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }

    static get observedAttributes() {
        return ["data-header", "data-footer"]
    }

    attributeChangedCallback(name, last, value) {
        if (last === value) return
        const key = name.replace("data-", "")
        if (["header", "footer"].includes(key)) this.shadowRoot.querySelector(`ui-${key}`).style.display = value !== "false" ? "flex" : "none"
    }
}

customElements.define("layout-main", MAIN)

export default MAIN
