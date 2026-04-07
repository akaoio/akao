import template from "./template.js"
import BaseRoute from "/UI/BaseRoute.js"

export class HOME extends BaseRoute {
    constructor() {
        super(template)
    }
}

customElements.define("route-home", HOME)

export default HOME
