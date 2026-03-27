/**
 * Sign data using the SEA key pair of a given role (or the current role).
 * @param {*} data - Data to sign
 * @param {string} [role] - Role whose pair to sign with (defaults to this.role)
 * @returns {Promise<string|undefined>} SEA signature, or undefined if unavailable
 */
export async function sign(data, role) {
    const { sea } = globalThis
    if (!sea) return

    const r = role ?? this.role
    const actor = r ? this.roles.get(r) : null
    if (!actor?.pair) return

    return await sea.sign(data, actor.pair)
}
