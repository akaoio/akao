/**
 * Derive a BIP-32 child key at a given index from an extended key or HD node.
 * Accepts either an xpub/xprv string or a live HDNodeWallet/HDNodeVoidWallet.
 * When a seed hex string is passed as the index, derives the 31-bit non-hardened
 * index via: parseInt(seedHex.substring(0, 8), 16) & 0x7fffffff
 *
 * Per white paper:
 *   VSE = root_SE.deriveChild(index_BE)
 *   index_BE = parseInt(seed_BE.substring(0, 8), 16) & 0x7fffffff
 *
 * @param {string|HDNodeWallet} extended - xpub, xprv string or live HD node
 * @param {number|string} indexOrSeed - Child index (integer) or seed hex (64 chars)
 * @returns {Promise<HDNodeWallet|HDNodeVoidWallet>} Derived child HD node
 */
export async function child(extended, indexOrSeed) {
    const { HDNodeWallet } = await import("ethers")
    const index =
        typeof indexOrSeed === "string"
            ? parseInt(indexOrSeed.substring(0, 8), 16) & 0x7fffffff
            : indexOrSeed
    const node = typeof extended === "string" ? HDNodeWallet.fromExtendedKey(extended) : extended
    return node.deriveChild(index)
}
