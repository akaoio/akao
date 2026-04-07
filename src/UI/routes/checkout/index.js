import template from "./template.js"
import BaseRoute from "/UI/BaseRoute.js"

export class CHECKOUT extends BaseRoute {
    constructor() {
        super(template)
    }
}

customElements.define("route-checkout", CHECKOUT)

export default CHECKOUT
