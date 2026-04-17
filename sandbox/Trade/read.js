import { readTradeRecord, resolveTradeId } from "./helpers.js"

// Merge trade state from both maker and taker Zen namespaces
// Both sides are public Zen user data — anyone with maker.pub + taker.pub can read
export async function read({ tradeId } = {}) {
    const resolvedTradeId = await resolveTradeId(this, tradeId)
    const pubs = [this.maker?.pub, this.taker?.pub, this.platform?.pub].filter(Boolean)
    const states = await Promise.all(
        pubs.map(pub => readTradeRecord({ zen: this.zen, pub, tradeId: resolvedTradeId }))
    )
    return Object.assign({}, ...states.filter(Boolean))
}
