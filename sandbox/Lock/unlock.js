import { HDNodeWallet } from "../../src/core/Ethers.js"

// Derives spending key from recipient's xprv + payer's index
// Caller must wipe the returned wallet from memory immediately after sendTransaction()
// WARNING: non-hardened derivation — never log or store the returned privateKey
export async function unlock(xprv) {
    if (!xprv) throw new Error("xprvRequired")
    const i = await this.index()
    return HDNodeWallet.fromExtendedKey(xprv).deriveChild(i)
}
