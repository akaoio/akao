import { html, render } from "/core/UI.js"
import States from "/core/States.js"
import SELECT from "/UI/components/select/index.js"

export class GEO extends HTMLElement {
    constructor() {
        super()
        this.states = new States({ country: null })
        this.attachShadow({ mode: "open" })
    }
}

customElements.define("ui-geo", GEO)

export default GEO