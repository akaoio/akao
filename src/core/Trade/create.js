import { putTradeRecord, resolveTradeId } from "./helpers.js"

// Taker accepts maker's order and authors the initial trade coordination record.
// Later transitions are appended by whichever actor owns that state change.
export async function create({ tradeId, matchedAt = Date.now(), taker = null } = {}) {
    const resolvedTradeId = await resolveTradeId(this, tradeId)
    const actor = taker || this.taker
    if (!actor?.pub || !actor?.pair) throw new Error("takerPairRequired")
    const record = { tradeId: resolvedTradeId, status: "matched", matched: true, matchedAt }
    await putTradeRecord({
        gun: this.gun,
        pub: actor.pub,
        tradeId: resolvedTradeId,
        fields: record,
        pair: actor.pair
    })
    return { ...record, writer: "taker" }
}
