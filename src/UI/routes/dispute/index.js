import template from "./template.js"
import BaseRoute from "/UI/BaseRoute.js"

export class DISPUTE extends BaseRoute {
    constructor() {
        super(template)
    }
}

customElements.define("route-dispute", DISPUTE)

export default DISPUTE
