import template from "./template.js"
import { render } from "/core/UI.js"
import { events } from "/core/Events.js"

export class HEADER extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }

    connectedCallback() {
        this.shadowRoot.querySelector(".games")?.addEventListener("click", () => {
            events.emit("game-nav:open")
        })
    }
}

customElements.define("ui-header", HEADER)
