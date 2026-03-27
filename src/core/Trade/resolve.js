/**
 * Transition the trade to "resolved" status (dispute resolved).
 * @param {Object} [data] - Optional additional state to merge
 * @returns {Trade} this (chainable)
 */
export function resolve(data) {
    this.state.set("status", "resolved")
    if (data && typeof data === "object")
        for (const [k, v] of Object.entries(data)) this.state.set(k, v)
    return this
}
