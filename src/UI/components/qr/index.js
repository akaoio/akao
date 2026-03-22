import { renderSVG } from "/core/QR.js"
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

    attributeChangedCallback(name, last, value) {
        if (last === value) return
        if (name === "data-value" && value && last !== value) this.render()
    }

    render() {
        const data = this.dataset.value
        const qr = this.shadowRoot.querySelector("#qr")
        if (!data) {
            qr.innerHTML = ""
            return
        }
        const code = renderSVG(data, { border: 0 })
        render(html`${code}`, qr)
    }
}

customElements.define("ui-qr", QR)