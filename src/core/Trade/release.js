/**
 * Transition the trade to "released" status (funds released to seller).
 * @param {Object} [data] - Optional additional state to merge
 * @returns {Trade} this (chainable)
 */
export function release(data) {
    this.state.set("status", "released")
    if (data && typeof data === "object")
        for (const [k, v] of Object.entries(data)) this.state.set(k, v)
    return this
}
