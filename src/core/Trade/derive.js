/**
 * Derive the full BIP-32 HD root node for a pair of roles and a scope.
 * Convenience wrapper: seed(roleA, roleB, scope) → root(seed).
 * Returns the full HDNodeWallet. Publish only the xpub (neuter().extendedKey).
 *
 * Per white paper:
 *   seed_SE = sha256(secret_SE + ":" + itemId)
 *   root_SE = HDNodeWallet.fromSeed(getBytes("0x" + seed_SE))
 *
 * @param {string} roleA - First role identifier
 * @param {string} roleB - Second role identifier
 * @param {string} [scope] - Domain separator (itemId, orderId, etc.)
 * @returns {Promise<HDNodeWallet|undefined>} BIP-32 HD root node, or undefined
 */
export async function derive(roleA, roleB, scope) {
    const s = await this.seed(roleA, roleB, scope)
    if (!s) return
    // root() is synchronous (static Ethers.js import) — no await needed
    return this.root(s)
}
