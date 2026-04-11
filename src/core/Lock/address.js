import { HDNodeWallet } from "../Ethers.js"

// Derives watch-only escrow address from recipient's xpub + payer's index
// Payer can verify on-chain but cannot spend (no xprv)
export async function address() {
    if (!this.recipient?.xpub) throw new Error("xpubRequired")
    const i = await this.index()
    const child = HDNodeWallet.fromExtendedKey(this.recipient.xpub).deriveChild(i)
    return child.address
}
