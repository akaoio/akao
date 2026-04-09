import template from "./template.js"
import Route from "/core/UI/Route.js"

export class HOME extends Route {
    static module = import.meta.url
    constructor() {
        super(template)
    }
}

customElements.define("route-home", HOME)

export default HOME
