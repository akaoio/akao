import { now } from "/core/Utils.js"
import ZEN from "/core/ZEN.js"

// Construct and return order
// The soul must be globally computable by everyone so that orders are discoverable.
// It must not be unique for each order, but rather represent a "group" of orders that can be easily found and filtered by others.
export function soul({ base, quote, side, candle } = {}) {
    // The expected soul format:
    // ZEN.pen() hex of: base asset (game item id), quote asset (currency id), order side,  candle number
    base = base?.id || this?.base?.id
    quote = quote?.id || this?.quote?.id
    candle = candle || now() // default to current time candle
    side = side || this?.side
    if (!base || !quote || !side) throw new Error("invalidInput")
    const key = { reg: 0 }
    const sep = ":"
    const timestamp = { tonum: { seg: { sep, idx: 0, of: key } } }
    const writer = { seg: { sep, idx: 1, of: key } }
    return ZEN.pen({
        key: {
            and: [
                {
                    let: {
                        bind: 0,
                        def: { divu: [timestamp, 300000] },
                        body: {
                            and: [
                                { gte: [{ reg: 128 }, candle] },
                                { lte: [{ reg: 128 }, candle] }
                            ]
                        }
                    }
                },
                { eq: [writer, { reg: 5 }] },
                { seg: { sep, idx: 2, of: key, match: { length: [1, 64] } } }
            ]
        },
        val:  { type: "string" },
        sign: true,
        pow:  { field: 0, difficulty: 3 },
        params: { base, side }
    })
}