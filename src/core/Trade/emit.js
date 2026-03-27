/**
 * Emit a named event to all registered route handlers.
 * @param {string} event - Event name
 * @param {*} [payload] - Payload forwarded to every handler
 * @returns {Trade} this (chainable)
 */
export function emit(event, payload) {
    const handlers = this.handlers.get(event)
    if (!handlers) return this
    for (const handler of handlers) handler(payload)
    return this
}
