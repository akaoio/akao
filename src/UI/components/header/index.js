import template from "./template.js"
import { render } from "/core/UI.js"

export class HEADER extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }

    connectedCallback() {
        this.shadowRoot.querySelector(".games")?.addEventListener("click", () => {
            window.dispatchEvent(new CustomEvent("game-nav:open"))
        })
    }
}

customElements.define("ui-header", HEADER)
