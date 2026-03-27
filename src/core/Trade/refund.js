/**
 * Transition the trade to "refunded" status (funds returned to buyer).
 * @param {Object} [data] - Optional additional state to merge
 * @returns {Trade} this (chainable)
 */
export function refund(data) {
    this.state.set("status", "refunded")
    if (data && typeof data === "object")
        for (const [k, v] of Object.entries(data)) this.state.set(k, v)
    return this
}
