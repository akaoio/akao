/**
 * BaseRoute - Specialized base class for route components
 * Extends BaseElement with shadow DOM setup and template rendering
 */
import { render } from "/core/UI.js"
import BaseElement from "./BaseElement.js"

export class BaseRoute extends BaseElement {
    constructor(template) {
        super()
        this.template = template
        this.attachShadow({ mode: "open" })
        if (template) render(template, this.shadowRoot)
    }

    /**
     * Query selector within shadow DOM (convenience method)
     */
    query(selector) {
        return this.shadowRoot.querySelector(selector)
    }

    /**
     * Query all within shadow DOM (convenience method)
     */
    queryAll(selector) {
        return this.shadowRoot.querySelectorAll(selector)
    }

    /**
     * Re-render template into shadow DOM
     * Useful for routes that need to update their template after fetch
     */
    renderTemplate(template = this.template) {
        if (template) render(template, this.shadowRoot)
    }
}

export default BaseRoute
