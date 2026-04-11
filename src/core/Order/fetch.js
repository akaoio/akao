import { soul } from "./soul.js"

// Query order book for a given item and type.
// Returns an array of two Gun observables: [current candle, previous candle].
// Querying both candles avoids missing orders posted just before a candle boundary.
// Callers should subscribe to both and merge results.
export function fetch({ gun, item, type, candle = Math.floor(Date.now() / 300000) } = {}) {
    const s = soul()
    const cur = gun.get(s).map({ ".": { "*": `${candle}:${item}:${type}:` } })
    const prev = gun.get(s).map({ ".": { "*": `${candle - 1}:${item}:${type}:` } })
    return [cur, prev]
}
