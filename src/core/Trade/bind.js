/**
 * Bind a role to an actor.
 * @param {string} role - Role identifier: "E" | "B" | "S" | "A"
 * @param {Object} actor - Actor object: { pub, pair? }
 * @returns {Trade} this (chainable)
 */
export function bind(role, actor) {
    if (!role || !actor) return this
    this.roles.set(role, actor)
    return this
}
