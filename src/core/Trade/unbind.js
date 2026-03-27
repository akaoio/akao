/**
 * Remove a role binding.
 * @param {string} role - Role identifier to remove
 * @returns {Trade} this (chainable)
 */
export function unbind(role) {
    this.roles.delete(role)
    this.keys.forEach((_, key) => {
        if (key.startsWith(role + ":") || key.endsWith(":" + role)) this.keys.delete(key)
    })
    return this
}
