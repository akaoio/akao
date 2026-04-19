import template from "./template.js"
import { render } from "/core/UI.js"

export class MODAL extends HTMLElement {
    static module = import.meta.url
    constructor() {
        super()
        this.template = template // Store for HMR
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.show = this.show.bind(this)
        this.showModal = this.showModal.bind(this)
        this.close = this.close.bind(this)
        this.toggle = this.toggle.bind(this)
        this.toggleModal = this.toggleModal.bind(this)
    }

    static get observedAttributes() {
        return ["data-header"]
    }

    attributeChangedCallback(name, last, value) {
        if (name !== "data-header" || last === value) return
        this.shadowRoot.querySelector("#header").dataset.key = `dictionary.${this.dataset.header}`
    }

    connectedCallback() {
        this.dialog = this.shadowRoot.querySelector("dialog")
        this.shadowRoot.querySelectorAll("dialog, .close, footer").forEach((el) => el.addEventListener("click", this.click))
        this.dialog.addEventListener("cancel", this.cancel)
        this.shadowRoot.querySelector("#header").dataset.key = `dictionary.${this.dataset.header}`
    }

    disconnectedCallback() {
        this.shadowRoot.querySelectorAll("dialog, .close, footer").forEach((el) => el.removeEventListener("click", this.click))
        this.dialog.removeEventListener("cancel", this.cancel)
    }

    click = (event) => {
        if (this._required) return
        if ([...this.shadowRoot.querySelectorAll("dialog, .close, footer")].includes(event.composedPath?.()?.[0])) this.dialog.close()
    }

    cancel = (event) => {
        if (this._required) event.preventDefault()
    }

    setRequired(required) {
        this._required = required
        const footer = this.shadowRoot.querySelector("footer")
        if (footer) footer.style.visibility = required ? "hidden" : ""
    }

    show() {
        this.dialog.show()
    }

    showModal() {
        this.dialog.showModal()
    }

    close() {
        this.dialog.close()
    }

    toggle() {
        this.dialog.open ? this.dialog.close() : this.dialog.show()
    }

    toggleModal() {
        this.dialog.open ? this.dialog.close() : this.dialog.showModal()
    }
}

customElements.define("ui-modal", MODAL)

export default MODAL
