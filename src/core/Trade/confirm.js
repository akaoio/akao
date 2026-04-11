import { sha256 } from "../Utils/crypto.js"
import { putTradeRecord, resolveRoles, resolveTradeId } from "./helpers.js"

// Buyer confirms receipt and reveals unlock indexes to seller and affiliate
// Writes to buyer's own Gun namespace — seller subscribes to read indexes
export async function confirm({ tradeId, buyerpair = null, buyer = null } = {}) {
    const resolvedTradeId = await resolveTradeId(this, tradeId)
    const buyerEntity = buyer || resolveRoles(this).buyer
    const pair = buyerpair || buyerEntity?.pair
    const { sea } = globalThis

    if (!sea) throw new Error("Trade: SEA not initialized")
    if (!buyerEntity?.pub || !pair) throw new Error("buyerPairRequired")

    const secret = await sea.secret(this.escrow.epub, pair)
    const index_TL = parseInt(sha256(secret + ":TL:" + resolvedTradeId).slice(0, 8), 16) & 0x7fffffff
    const index_CL = parseInt(sha256(secret + ":CL:" + resolvedTradeId).slice(0, 8), 16) & 0x7fffffff
    const fields = {
        confirmed: true,
        confirmedAt: Date.now(),
        status: "confirmed",
        unlock_index_TL: index_TL,
        unlock_index_CL: index_CL
    }

    await putTradeRecord({
        gun: this.gun,
        pub: buyerEntity.pub,
        tradeId: resolvedTradeId,
        fields,
        pair
    })

    return { tradeId: resolvedTradeId, ...fields }
}
