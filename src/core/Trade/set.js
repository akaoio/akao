/**
 * Set multiple state values from a plain object.
 * @param {Object} [data] - Key/value pairs to store
 * @returns {Trade} this (chainable)
 */
export function set(data = {}) {
    if (!data || typeof data !== "object") return this
    for (const [k, v] of Object.entries(data)) this.state.set(k, v)
    return this
}
