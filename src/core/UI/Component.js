import { render } from "/core/UI.js"

/**
 * Component - Base class for all custom elements
 * Automatically tracked by HMR interceptor (no manual registration needed)
 * Centralizes lifecycle, subscription management, and event listener cleanup
 */
export class Component extends HTMLElement {
    constructor() {
        super()
        this.subs = []
        this.template = null // Store template for HMR re-rendering
    }

    /**
     * Render template to shadow DOM (called by HMR for hot-reloading)
     * Subclasses can override to customize rendering behavior
     */
    async render() {
        if (!this.template || !this.shadowRoot) return
        render(this.template, this.shadowRoot)
        
        // Re-run onconnect after re-render to reinitialize component state
        if (typeof this.onconnect === 'function') {
            this.onconnect()
        }
    }

    /**
     * Register cleanup functions to run on disconnectedCallback
     * @param {...Function} fns - Functions to call on disconnect
     */
    sub(...fns) {
        this.subs.push(...fns.filter(Boolean))
    }

    /**
     * Attach event listener with automatic cleanup on disconnect
     * @param {EventTarget} target - Element or object that emits events
     * @param {string} event - Event name
     * @param {Function} handler - Event handler
     * @param {Object} options - addEventListener options
     * @returns {Function} The unsubscribe function (also auto-registered)
     */
    listen(target, event, handler, options) {
        if (!target) return
        target.addEventListener(event, handler, options)
        const off = () => target.removeEventListener(event, handler, options)
        this.subs.push(off)
        return off
    }

    /**
     * Subscribe to Context or States updates with automatic cleanup
     * @param {Object} observable - Context or States instance
     * @param {string|Array} key - Key(s) to watch
     * @param {Function} callback - Callback when value changes
     * @param {boolean} now - Trigger callback immediately
     * @returns {Function} The unsubscribe function (also auto-registered)
     */
    watch(observable, key, callback, now = false) {
        if (!observable || !observable.on) return
        const off = observable.on(key, callback, now)
        this.subs.push(off)
        return off
    }

    /**
     * Query selector within shadow DOM (if exists)
     */
    query(sel) {
        return this.shadowRoot?.querySelector(sel)
    }

    /**
     * Query all within shadow DOM (if exists)
     */
    queryall(sel) {
        return this.shadowRoot?.querySelectorAll(sel)
    }

    /**
     * Called when element connects to DOM (override in subclasses)
     */
    onconnect() {}

    /**
     * Called when element disconnects from DOM (override in subclasses)
     */
    ondisconnect() {}

    /**
     * Standard lifecycle hook
     */
    connectedCallback() {
        this.onconnect()
    }

    /**
     * Standard lifecycle hook - auto-cleanup subscriptions
     */
    disconnectedCallback() {
        this.ondisconnect()
        this.subs.forEach(off => off())
        this.subs.length = 0
    }
}

export default Component
