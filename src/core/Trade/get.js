/**
 * Get a value from the trade state.
 * If no key is provided, returns the full state as a plain object.
 * @param {string} [key] - State key to retrieve
 * @returns {*} Stored value, or full state object if no key given
 */
export function get(key) {
    if (key === undefined) return Object.fromEntries(this.state)
    return this.state.get(key)
}
