import { putTradeRecord, resolveRoles, resolveTradeId } from "./helpers.js"

// Seller marks item as delivered — writes to own Gun namespace
export async function deliver({ tradeId, seller = null } = {}) {
    const resolvedTradeId = await resolveTradeId(this, tradeId)
    const sellerEntity = seller || resolveRoles(this).seller

    if (!sellerEntity?.pub || !sellerEntity?.pair) throw new Error("sellerPairRequired")

    const fields = { delivered: true, deliveredAt: Date.now(), status: "delivered" }
    await putTradeRecord({
        gun: this.gun,
        pub: sellerEntity.pub,
        tradeId: resolvedTradeId,
        fields,
        pair: sellerEntity.pair
    })
    return { tradeId: resolvedTradeId, ...fields }
}
