/**
 * Remove a key from the trade state.
 * @param {string} key - State key to remove
 * @returns {Trade} this (chainable)
 */
export function del(key) {
    this.state.delete(key)
    return this
}
