import template from "./template.js"
import { render, html } from "/core/UI.js"
import { Elements } from "/core/Stores.js"
import QRCode from "/core/QR.js"

export class DEPOSIT extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
    }

    async connectedCallback() {
        this.$wallets = this.shadowRoot.querySelector("ui-wallets")
        this.subscriptions.push(
            this.$wallets.states.on("address", async ({ value }) => {
                if (!value) return
                const code = await QRCode.toString(value, { type: "svg", margin: 0 })
                render(html`${code}`, this.shadowRoot.querySelector("#qr"))
            }, true)
        )
        Elements.Access?.checkpoint()
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }
}

customElements.define("route-deposit", DEPOSIT)

export default DEPOSIT
