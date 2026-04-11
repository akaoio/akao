import template from "./template.js"
import Route from "/core/UI/Route.js"

export class DISPUTE extends Route {
    static module = import.meta.url
    constructor() {
        super(template)
    }
}

customElements.define("route-dispute", DISPUTE)

export default DISPUTE
