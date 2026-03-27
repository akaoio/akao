/**
 * Encrypt a payload using SEA.
 * If roleA and roleB are provided, encrypts with the shared ECDH secret.
 * Otherwise encrypts with the current role's key pair.
 * @param {*} data - Data to encrypt
 * @param {string} [roleA] - First role for shared-secret encryption
 * @param {string} [roleB] - Second role for shared-secret encryption
 * @returns {Promise<string|undefined>} Encrypted ciphertext, or undefined
 */
export async function wrap(data, roleA, roleB) {
    const { sea } = globalThis
    if (!sea) return

    if (roleA && roleB) {
        const secret = await this.pair(roleA, roleB)
        if (!secret) return
        return await sea.encrypt(data, secret)
    }

    const actor = this.role ? this.roles.get(this.role) : null
    if (actor?.pair) return await sea.encrypt(data, actor.pair)
}
