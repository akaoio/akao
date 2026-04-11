import template from "./template.js"
import Route from "/core/UI/Route.js"
import { Elements } from "/core/Stores.js"

export class DEPOSIT extends Route {
    static module = import.meta.url
    constructor() {
        super(template)
    }

    async onconnect() {
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
