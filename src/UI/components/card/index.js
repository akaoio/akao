import template from "./template.js"
import { render } from "/core/UI.js"
import { Context } from "/core/Context.js"

export class CARD extends HTMLElement {
    static module = import.meta.url

    constructor() {
        super()
        this.template = template
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }

    connectedCallback() {
        this._sync()
        this._unsub = Context.on("dictionary", () => this._sync())
    }

    disconnectedCallback() {
        if (this._unsub) this._unsub()
    }

    static get observedAttributes() {
        return ["data-title", "data-meta"]
    }

    attributeChangedCallback() {
        this._sync()
    }

    _sync() {
        const $title = this.shadowRoot.querySelector("#title")
        const $meta  = this.shadowRoot.querySelector("#meta")
        const dict   = Context.get("dictionary") || {}
        const resolve = (key) => key ? (dict[key] ?? key) : ""
        if ($title) $title.textContent = resolve(this.dataset.title)
        if ($meta)  $meta.textContent  = resolve(this.dataset.meta)
    }
}

customElements.define("ui-card", CARD)

export default CARD
