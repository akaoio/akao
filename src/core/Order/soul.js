import { now } from "/core/Utils.js"
import ZEN from "/core/ZEN.js"

// Construct and return order soul
// The soul must be globally computable by everyone so that orders are discoverable.
// It must not be unique for each order, but rather represent a "group" of orders that can be easily found and filtered by others.
export function soul({ candle, side, base, quote } = {}) {
    candle = candle || now(300000) // default to current 5-minute candle
    side = side || this?.side
    base = base?.id || this?.base?.id
    quote = quote?.id || this?.quote?.id
    if (!side || !base || !quote) throw new Error("invalidInput")
    const sep = ":"
    // key format: timestamp:pub/epub:side:type/game_or_chain/id/qty  (see Order/id.js)
    // seg 0: raw Date.now() timestamp — divide by 300000 to get 5-min candle number
    const timestamp = { tonum: { seg: { sep, idx: 0 } } }
    const key_candle = { divu: [timestamp, 300000] }
    // seg 1: pub/epub, extract pub (idx 0 split by "/")
    const maker_pub = { seg: { sep: "/", of: { seg: { sep, idx: 1 } }, idx: 0 } }
    // seg 2: side
    const $side = { seg: { sep, idx: 2 } }
    // seg 3: type/game_or_chain/id/qty, extract id (idx 2 split by "/")
    const $base = { seg: { sep: "/", of: { seg: { sep, idx: 3 } }, idx: 2 } }
    // val format: type/category/id:quantity[:affiliate]
    // seg 0 (split by ":") = type/category/id, then split by "/" idx 2 = id
    const $quote = { seg: { sep: "/", of: { seg: { sep, of: { reg: 1 }, idx: 0 } }, idx: 2 } }
    return ZEN.pen({
        key: {
            and: [
                { eq: [key_candle, candle] },
                { eq: [maker_pub, { reg: 5 }] },
                { eq: [$side, side] },
                { eq: [$base, base] }
            ]
        },
        val: {
            and: [
                { eq: [$quote, quote] }
            ]
        },
        sign: true,
        pow: this.pow
    })
}
