/**
 * Serialize the current trade state into a plain JSON-safe object.
 * Only public-safe actor data (pub) is included — never the key pair.
 * @returns {Object} Snapshot of id, scope, state, and roles (pub only)
 */
export function snapshot() {
    const roles = {}
    for (const [role, actor] of this.roles)
        roles[role] = { pub: actor.pub }

    return {
        id: this.id,
        scope: this.scope,
        role: this.role,
        state: Object.fromEntries(this.state),
        roles,
    }
}
