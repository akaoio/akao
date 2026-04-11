import { putTradeRecord, resolveTradeId } from "./helpers.js"

// Write initial trade records to all available authenticated participant namespaces.
// Each side can later extend only its own record.
export async function create({ tradeId, matchedAt = Date.now() } = {}) {
    const resolvedTradeId = await resolveTradeId(this, tradeId)
    const record = { tradeId: resolvedTradeId, status: "matched", matchedAt }
    const writers = [
        this.maker?.pair ? { pub: this.maker.pub, pair: this.maker.pair, role: "maker" } : null,
        this.taker?.pair ? { pub: this.taker.pub, pair: this.taker.pair, role: "taker" } : null
    ].filter(Boolean)

    if (!writers.length) throw new Error("writerPairRequired")

    await Promise.all(
        writers.map(({ pub, pair }) => putTradeRecord({
            gun: this.gun,
            pub,
            tradeId: resolvedTradeId,
            fields: record,
            pair
        }))
    )

    return { ...record, writers: writers.map(({ role }) => role) }
}
