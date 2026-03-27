import { sha256 } from "../Utils/crypto.js"

/**
 * Derive a deterministic 32-byte seed from a pair of roles and an optional scope.
 * Combines the ECDH shared secret with the scope via sha256.
 *
 * Per white paper:
 *   seed_SE = sha256(secret_SE + ":" + itemId)
 *   seed_BE = sha256(secret_BE + ":" + orderId)
 *
 * Result is cached in this.keys under "A:B" key when no scope is provided.
 * @param {string} roleA - First role identifier
 * @param {string} roleB - Second role identifier
 * @param {string} [scope] - Domain separator (itemId, orderId, etc.)
 * @returns {Promise<string|undefined>} 64-char hex seed, or undefined if unavailable
 */
export async function seed(roleA, roleB, scope) {
    const key = `${roleA}:${roleB}`
    const cached = this.keys.get(key)
    // Return cached seed only when no scope is requested
    if (!scope && cached?.seed) return cached.seed

    const secret = await this.pair(roleA, roleB)
    if (!secret) return

    const input = scope ? `${secret}:${scope}` : secret
    const derived = sha256(input)

    if (!scope) this.keys.set(key, { ...this.keys.get(key), seed: derived })
    return derived
}
