/**
 * Transition the trade to "locked" status (funds in escrow).
 * @param {Object} [data] - Optional additional state to merge
 * @returns {Trade} this (chainable)
 */
export function lock(data) {
    this.state.set("status", "locked")
    if (data && typeof data === "object")
        for (const [k, v] of Object.entries(data)) this.state.set(k, v)
    return this
}
