/**
 * Universal event system that works in both browser and Node.js environments.
 * Abstracts platform-specific event handling differences.
 */

import { NODE, BROWSER } from "./Utils/environment.js"

// Load EventEmitter class once at module level (Node.js only)
let EventEmitterClass = null
if (NODE && !BROWSER) {
    const { EventEmitter } = await import("events")
    EventEmitterClass = EventEmitter
}

export class Events {
    constructor(target = null) {
        // Each instance gets its own independent event dispatcher
        if (target) this.$events = target
        else if (BROWSER && !NODE) this.$events = new EventTarget()
        else if (NODE && !BROWSER) this.$events = new EventEmitterClass()
    }

    /**
     * Register an event listener with automatic unsubscribe capability.
     * Works across browser (addEventListener) and Node.js (on) environments.
     *
     * @param {string} event - The event name to listen for
     * @param {Function} listener - Callback function to execute when event fires
     * @returns {Function} Unsubscribe function that removes the listener
     */
    on(event, listener, options) {
        if (BROWSER && !NODE) this.$events.addEventListener(event, listener, options)
        else if (NODE && !BROWSER) this.$events.on(event, listener)
        // Return unsubscribe function with self-reference for convenience
        const off = () => this.off(event, listener, options)
        off.off = off
        return off
    }

    once(event, listener, options) {
        if (BROWSER && !NODE) {
            const browserOptions = typeof options === "object" && options !== null ? options : {}
            return this.on(event, listener, { ...browserOptions, once: true })
        }

        if (NODE && !BROWSER) this.$events.once(event, listener)
        const off = () => this.off(event, listener)
        off.off = off
        return off
    }

    /**
     * Unregister an event listener.
     * Handles platform differences between browser and Node.js APIs.
     *
     * @param {string} event - The event name to stop listening for
     * @param {Function} listener - The callback function to remove
     */
    off(event, listener, options) {
        if (BROWSER && !NODE) this.$events.removeEventListener(event, listener, options)
        else if (NODE && !BROWSER) this.$events.removeListener(event, listener)
    }

    /**
     * Emit/dispatch an event to all registered listeners.
     * Automatically wraps detail in an object for consistency across platforms.
     *
     * @param {string} event - The event name to emit
     * @param {*} detail - Event payload/data to pass to listeners
     */
    emit(event, detail, options = {}) {
        // Browser: create and dispatch CustomEvent
        if (BROWSER && !NODE) {
            const e = new CustomEvent(event, {
                detail,
                bubbles: options?.bubbles,
                composed: options?.composed,
                cancelable: options?.cancelable
            })
            this.$events.dispatchEvent(e)
            return e
        }
        // Node.js: emit event with detail as argument
        else if (NODE && !BROWSER) return this.$events.emit(event, { detail })
    }
}

export default Events

// Create or reuse global Events singleton for app-wide event bus
globalThis.events = globalThis.events || new Events()

// Export the global event instance for convenient access throughout the app
export const events = globalThis.events
