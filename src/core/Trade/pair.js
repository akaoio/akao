/**
 * Compute the ECDH shared secret between two roles.
 * Uses SEA's Diffie-Hellman: sea.secret(their.pub, my.pair)
 * The result is symmetric: pair("A", "B") === pair("B", "A")
 * Result is cached in this.keys under "A:B" key.
 * @param {string} roleA - First role identifier
 * @param {string} roleB - Second role identifier
 * @returns {Promise<string|undefined>} Shared secret hex string, or undefined if unavailable
 */
export async function pair(roleA, roleB) {
    const key = `${roleA}:${roleB}`
    const cached = this.keys.get(key)
    if (cached?.secret) return cached.secret

    const { sea } = globalThis
    if (!sea) return

    const a = this.roles.get(roleA)
    const b = this.roles.get(roleB)
    if (!a || !b) return

    let secret
    // SEA uses separate encryption key pairs (epub/epriv) for ECDH via sea.secret().
    // Always prefer epub over pub for sea.secret() because SEA's ECDH operates on the
    // encryption curve key pair, not the signing key pair. Falling back to pub is kept
    // for compatibility with actors that expose only a single key field.
    if (a.pair) secret = await sea.secret(b.epub ?? b.pub, a.pair)
    else if (b.pair) secret = await sea.secret(a.epub ?? a.pub, b.pair)
    if (!secret) return

    this.keys.set(key, { ...cached, secret })
    return secret
}
