import { soul } from "./soul.js"

// Query order book for a given base item and side.
// Returns an array of two Gun observable maps: [current candle soul, previous candle soul].
// Querying both souls avoids missing orders posted just before a candle boundary.
export function fetch({ gun, baseId, side, candle = Math.floor(Date.now() / 300000) } = {}) {
    const cur = gun.get(soul({ baseId, side, candle })).map()
    const prev = gun.get(soul({ baseId, side, candle: candle - 1 })).map()
    return [cur, prev]
}
