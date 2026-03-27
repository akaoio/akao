/**
 * Transition the trade to "settled" status.
 * @param {Object} [data] - Optional additional state to merge
 * @returns {Trade} this (chainable)
 */
export function settle(data) {
    this.state.set("status", "settled")
    if (data && typeof data === "object")
        for (const [k, v] of Object.entries(data)) this.state.set(k, v)
    return this
}
