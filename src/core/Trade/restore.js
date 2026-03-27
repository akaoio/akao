/**
 * Restore trade state from a previously captured snapshot.
 * Roles restored from a snapshot will have only pub (no key pair).
 * Use bind() after restore to re-attach pairs where needed.
 * @param {Object} [data] - Snapshot object (from snapshot())
 * @returns {Trade} this (chainable)
 */
export function restore(data = {}) {
    if (!data || typeof data !== "object") return this
    if (data.id != null) this.id = data.id
    if (data.scope != null) this.scope = data.scope
    if (data.role != null) this.role = data.role
    if (data.state && typeof data.state === "object")
        for (const [k, v] of Object.entries(data.state)) this.state.set(k, v)
    if (data.roles && typeof data.roles === "object")
        for (const [role, actor] of Object.entries(data.roles)) this.roles.set(role, actor)
    return this
}
