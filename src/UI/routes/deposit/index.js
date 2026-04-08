import template from "./template.js"
import BaseRoute from "/UI/BaseRoute.js"
import { Elements } from "/core/Stores.js"

export class DEPOSIT extends BaseRoute {
    constructor() {
        super(template)
    }

    async onConnect() {
        this.$wallets = this.shadowRoot.querySelector("ui-wallets")
        this.watch(this.$wallets.states, "address", async ({ value }) => {
            const $qr = this.shadowRoot.querySelector("ui-qr")
            if (!value) return
            $qr.dataset.value = value
        }, true)
        Elements.Access?.checkpoint()
    }
}

customElements.define("route-deposit", DEPOSIT)

export default DEPOSIT
