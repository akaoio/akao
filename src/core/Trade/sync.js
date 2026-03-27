/**
 * Merge an external state object into the trade's internal state map.
 * Useful for applying remote updates or hydrating from a peer snapshot.
 * @param {Object} [data] - Key/value pairs to merge into this.state
 * @returns {Trade} this (chainable)
 */
export function sync(data = {}) {
    if (!data || typeof data !== "object") return this
    for (const [k, v] of Object.entries(data)) this.state.set(k, v)
    return this
}
