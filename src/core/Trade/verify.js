/**
 * Verify a SEA signature against the public key of a given role.
 * @param {string} signed - Signed data (SEA signature string)
 * @param {string} [role] - Role whose pub key to verify against (defaults to this.role)
 * @returns {Promise<*|undefined>} Original data if valid, undefined otherwise
 */
export async function verify(signed, role) {
    const { sea } = globalThis
    if (!sea) return

    const r = role ?? this.role
    const actor = r ? this.roles.get(r) : null
    if (!actor?.pub) return

    return await sea.verify(signed, actor.pub)
}
