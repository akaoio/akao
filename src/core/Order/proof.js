import { sha256 } from "../Utils/crypto.js"

// Verify Fund Proof wallet has sufficient balance (buy orders only)
// Must be called by Taker at match time — not just at discovery time
// FP wallet: root_M.deriveChild(index where index = sha256("FP:" + orderId))
export async function proof({ chain, orderId, xpub, required }) {
    const index = parseInt(sha256("FP:" + orderId).slice(0, 8), 16) & 0x7fffffff
    const { HDNodeWallet } = await import("ethers")
    const fp = HDNodeWallet.fromExtendedKey(xpub).deriveChild(index)
    const balance = await chain.getBalance(fp.address)
    return { address: fp.address, balance, sufficient: balance >= required }
}
