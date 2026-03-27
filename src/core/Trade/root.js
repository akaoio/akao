import { HDNodeWallet, getBytes } from "../Ethers.js"

/**
 * Derive a BIP-32 HD root wallet from a 32-byte seed hex string.
 * Returns a full HDNodeWallet (with private key) from the given seed.
 * The caller must hold this privately — never transmit the resulting node.
 *
 * Per white paper:
 *   root_SE = HDNodeWallet.fromSeed(getBytes("0x" + seed_SE))
 *
 * @param {string} seedHex - 64-char hex string (32 bytes)
 * @returns {HDNodeWallet} BIP-32 HD root node
 */
export function root(seedHex) {
    return HDNodeWallet.fromSeed(getBytes("0x" + seedHex))
}
