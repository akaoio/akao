import QRCode from "/core/QR.js"
import template from "./template.js"
import { html, render } from "/core/UI.js"

class QR extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        this.render = this.render.bind(this)
        render(template, this.shadowRoot)
    }

    static get observedAttributes() {
        return ["data-value"]
    }

    attributeChangedCallback(name, oldValue, newValue) {
        if (name === "data-value") this.render()
    }

    async render() {
        const data = this.dataset.value
        const qr = this.shadowRoot.querySelector("#qr")
        if (!data) {
            qr.innerHTML = ""
            return
        }
        const code = await QRCode.toString(data, { type: "svg", margin: 0 })
        render(html`${code}`, qr)
    }
}

customElements.define("ui-qr", QR)