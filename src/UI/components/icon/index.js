import template from "./template.js"
import { render } from "/core/UI.js"

export class ICON extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }

    static get observedAttributes() {
        return ["data-icon"]
    }

    connectedCallback() {
        if (!this.dataset.icon) return
        this.render()
    }

    attributeChangedCallback(name, last, value) {
        if (name !== "data-icon") return
        this.render()
    }

    render() {
        let src = this.dataset.icon
        const svg = this.shadowRoot.querySelector("ui-svg")
        if (src) {
            if (!src.includes("/")) src = `/images/icons/${src}.svg`
            svg.dataset.src = src
        } else delete svg.dataset.src
    }
}

customElements.define("ui-icon", ICON)

export default ICON
