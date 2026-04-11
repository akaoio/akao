import { readTradeRecord, resolveTradeId } from "./helpers.js"

// Merge trade state from both maker and taker Gun namespaces
// Both sides are public Gun user data — anyone with maker.pub + taker.pub can read
export async function read({ tradeId } = {}) {
    const resolvedTradeId = await resolveTradeId(this, tradeId)
    const pubs = [this.maker?.pub, this.taker?.pub, this.escrow?.pub].filter(Boolean)
    const states = await Promise.all(
        pubs.map(pub => readTradeRecord({ gun: this.gun, pub, tradeId: resolvedTradeId }))
    )
    return Object.assign({}, ...states.filter(Boolean))
}
