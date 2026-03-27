/**
 * Register a handler for a named event/message type.
 * Returns an unsubscribe function.
 * @param {string} event - Event name to listen for
 * @param {Function} handler - Callback invoked with the payload
 * @returns {Function} Unsubscribe function
 */
export function route(event, handler) {
    if (!this.handlers.has(event)) this.handlers.set(event, new Set())
    if (typeof handler === "function") this.handlers.get(event).add(handler)
    return () => this.handlers.get(event)?.delete(handler)
}
