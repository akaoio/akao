/**
 * BaseElement - Shared base class for all custom elements
 * Centralizes lifecycle, subscription management, and event listener cleanup
 */
export class BaseElement extends HTMLElement {
    constructor() {
        super()
        this.subscriptions = []
    }

    /**
     * Register cleanup functions to run on disconnectedCallback
     * @param {...Function} offFns - Functions to call on disconnect (e.g., event listener removers, unsubscribers)
     */
    subscribe(...offFns) {
        this.subscriptions.push(...offFns.filter(Boolean))
    }

    /**
     * Attach event listener with automatic cleanup on disconnect
     * @param {EventTarget} target - Element or object that emits events
     * @param {string} event - Event name
     * @param {Function} handler - Event handler
     * @param {Object} options - addEventListener options (capture, once, passive, etc.)
     * @returns {Function} The unsubscribe function (also auto-registered)
     */
    listen(target, event, handler, options) {
        if (!target) return
        target.addEventListener(event, handler, options)
        const off = () => target.removeEventListener(event, handler, options)
        this.subscriptions.push(off)
        return off
    }

    /**
     * Subscribe to Context or States updates with automatic cleanup
     * @param {Object} observable - Context or States instance
     * @param {string|Array} key - Key(s) to watch
     * @param {Function} callback - Callback when value changes
     * @param {boolean} immediate - Trigger callback immediately
     * @returns {Function} The unsubscribe function (also auto-registered)
     */
    watch(observable, key, callback, immediate = false) {
        if (!observable || !observable.on) return
        const off = observable.on(key, callback, immediate)
        this.subscriptions.push(off)
        return off
    }

    /**
     * Called when element connects to DOM (override in subclasses)
     * Guaranteed to run after constructor
     */
    onConnect() {}

    /**
     * Called when element disconnects from DOM (override in subclasses)
     * Runs before auto-cleanup of subscriptions
     */
    onDisconnect() {}

    /**
     * Standard lifecycle hook - calls onConnect then auto-cleanup on disconnect
     */
    connectedCallback() {
        this.onConnect()
    }

    /**
     * Standard lifecycle hook - runs onDisconnect then clears all subscriptions
     */
    disconnectedCallback() {
        this.onDisconnect()
        this.subscriptions.forEach(off => off())
        this.subscriptions.length = 0
    }
}

export default BaseElement
