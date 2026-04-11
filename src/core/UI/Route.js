/**
 * Route - Base class for route components
 * Extends Component with shadow DOM setup and template rendering
 */
import { render } from "/core/UI.js"
import Component from "./Component.js"

export class Route extends Component {
    constructor(tpl) {
        super()
        this.template = tpl // Store for HMR
        this.tpl = tpl      // Keep for backward compatibility
        this.attachShadow({ mode: "open" })
        if (tpl) render(tpl, this.shadowRoot)
    }

    /**
     * Re-render template into shadow DOM (HMR-compatible)
     */
    async render() {
        if (!this.template || !this.shadowRoot) return
        render(this.template, this.shadowRoot)
        
        // Re-run onconnect after re-render
        if (typeof this.onconnect === 'function') {
            this.onconnect()
        }
    }

    /**
     * Legacy method for backward compatibility
     */
    rendertpl(tpl = this.template) {
        if (tpl) render(tpl, this.shadowRoot)
    }
}

export default Route
