import { sha256 } from "../Utils/crypto.js"
import { HDNodeWallet } from "../Ethers.js"

// Verify Fund Proof wallet has sufficient balance (buy orders only).
// Must be called by Taker at match time — not just at discovery time.
// FP wallet: root_M.deriveChild(index) where index = sha256("FP:" + orderId) & 0x7fffffff
//
// IMPORTANT: chain.getBalance() returns native token balance as BigInt (wei).
// `required` must be passed in the same unit (wei / token atomic units).
// For ERC-20 tokens (e.g. USDT), the caller must use the token contract's
// balanceOf() instead of getBalance() — this function covers native-token proofs only.
export async function proof({ chain, orderId, xpub, required }) {
    if (!required || BigInt(required) <= 0n) return { error: "invalidRequired" }
    const index = parseInt(sha256("FP:" + orderId).slice(0, 8), 16) & 0x7fffffff
    const fp = HDNodeWallet.fromExtendedKey(xpub).deriveChild(index)
    const balance = await chain.getBalance(fp.address)
    const req = BigInt(required)
    return { address: fp.address, balance, sufficient: balance >= req }
}
