/**
 * Open a trade (set status to "open") or decrypt an encrypted payload.
 * - open()           → sets trade status to "open"
 * - open({...})      → sets status to "open" and merges provided data into state
 * - open("cipher")   → decrypts ciphertext using SEA; roleA/roleB select shared secret
 * @param {string|Object} [data] - Encrypted string to decrypt, or initial state data
 * @param {string} [roleA] - First role for shared-secret decryption
 * @param {string} [roleB] - Second role for shared-secret decryption
 * @returns {Trade|*} this for state transition, or decrypted value
 */
export async function open(data, roleA, roleB) {
    // Decrypt mode: called with a ciphertext string
    if (typeof data === "string") {
        const { sea } = globalThis
        if (!sea) return
        if (roleA && roleB) {
            const secret = await this.pair(roleA, roleB)
            if (!secret) return
            return await sea.decrypt(data, secret)
        }
        const actor = this.role ? this.roles.get(this.role) : null
        if (actor?.pair) return await sea.decrypt(data, actor.pair)
        return
    }
    // State transition: open the trade
    this.state.set("status", "open")
    if (data && typeof data === "object")
        for (const [k, v] of Object.entries(data)) this.state.set(k, v)
    return this
}
