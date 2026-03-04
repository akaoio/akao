import template from "./template.js"
import { render } from "/core/UI.js"

export class PROFILE extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }
}

customElements.define("route-profile", PROFILE)

export default PROFILE
