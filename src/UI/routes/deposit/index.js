import template from "./template.js"
import { render } from "/core/UI.js"
import { Elements } from "/core/Stores.js"

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
                const $qr = this.shadowRoot.querySelector("ui-qr")
                if (!value) return
                $qr.dataset.value = value
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
