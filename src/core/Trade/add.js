/**
 * Add a single key/value entry to the trade state.
 * @param {string} key - State key
 * @param {*} value - Value to store
 * @returns {Trade} this (chainable)
 */
export function add(key, value) {
    this.state.set(key, value)
    return this
}
