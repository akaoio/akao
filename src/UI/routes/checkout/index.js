import template from "./template.js"
import Route from "/core/UI/Route.js"

export class CHECKOUT extends Route {
    static module = import.meta.url
    constructor() {
        super(template)
    }
}

customElements.define("route-checkout", CHECKOUT)

export default CHECKOUT
