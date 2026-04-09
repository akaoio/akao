import { sha256 } from "../Utils/crypto.js"

// Buyer confirms receipt and reveals unlock indexes to seller and affiliate
// Writes to buyer's own Gun namespace — seller subscribes to read indexes
export async function confirm({ tradeId, buyerpair }) {
    const { sea } = globalThis
    const secret = await sea.secret(this.escrow.epub, buyerpair)
    const index_TL = parseInt(sha256(secret + ":TL:" + tradeId).slice(0, 8), 16) & 0x7fffffff
    const index_CL = parseInt(sha256(secret + ":CL:" + tradeId).slice(0, 8), 16) & 0x7fffffff
    await this.gun.user().get("trades").get(tradeId).put({
        confirmed: true,
        unlock_index_TL: index_TL,
        unlock_index_CL: index_CL
    })
}
