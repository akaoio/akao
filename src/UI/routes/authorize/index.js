import template from "./template.js"
import { render } from "/core/UI.js"

export class AUTHORIZE extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }
}

customElements.define("ui-authorize", AUTHORIZE)

export default AUTHORIZE