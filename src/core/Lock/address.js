// Derives watch-only escrow address from recipient's xpub + payer's index
// Payer can verify on-chain but cannot spend (no xprv)
export async function address() {
    const { HDNodeWallet } = await import("ethers")
    const i = await this.index()
    const child = HDNodeWallet.fromExtendedKey(this.recipient.xpub).deriveChild(i)
    return child.address
}
