import template from "./template.js"
import { render } from "/core/UI.js"
import Component from "/core/UI/Component.js"

export class LABEL extends Component {
    static module = import.meta.url

    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }

    static get observedAttributes() {
        return ["data-key"]
    }

    onconnect() {
        this._sync()
    }

    attributeChangedCallback(name, last, value) {
        if (name !== "data-key" || last === value) return
        this._sync()
    }

    _sync() {
        const ctx = this.shadowRoot.querySelector("ui-context")
        if (!ctx) return
        const key = this.dataset.key
        if (key) ctx.dataset.key = key
        else delete ctx.dataset.key
    }
}

customElements.define("ui-label", LABEL)

export default LABEL
