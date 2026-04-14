import { soul } from "./soul.js"
import zen from "../ZEN.js"

// Query order book for a given base item and side.
// Returns an array of two Gun observable maps: [current candle soul, previous candle soul].
// Querying both souls avoids missing orders posted just before a candle boundary.
export function fetch({ baseId, side, candle = Math.floor(Date.now() / 300000) } = {}) {
    const cur = zen.get(soul({ baseId, side, candle })).map()
    const prev = zen.get(soul({ baseId, side, candle: candle - 1 })).map()
    return [cur, prev]
}
