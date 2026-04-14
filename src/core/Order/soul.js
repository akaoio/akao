import zen from "../ZEN.js"

// Returns the Pen soul string for one market window:
// params = { baseId, side, candle }
// key    = <timestamp>:<pub>:<nonce>
//
// The soul identity changes when baseId/side/candle change because they are encoded
// in SEA.pen({ params }). Validation remains generic and reusable:
//   seg 0 — timestamp whose derived candle must equal the soul candle
//   seg 1 — full writer pub and must equal R[5]
//   seg 2 — nonce (PoW uniqueness)
export function soul({ baseId = this?.base?.id, side = this?.side, candle = Math.floor(Date.now() / 300000) } = {}) {
    if (!baseId || !side) throw new Error("invalidInput")
    const stamp = { tonum: { seg: { sep: ":", idx: 0, of: { reg: 0 } } } }
    const writer = { seg: { sep: ":", idx: 1, of: { reg: 0 } } }
    return zen.pen({
        key: {
            and: [
                {
                    let: {
                        bind: 0,
                        def: { divu: [stamp, 300000] },
                        body: {
                            and: [
                                { gte: [{ reg: 128 }, candle] },
                                { lte: [{ reg: 128 }, candle] }
                            ]
                        }
                    }
                },
                { eq: [writer, { reg: 5 }] },
                { seg: { sep: ":", idx: 2, of: { reg: 0 }, match: { length: [1, 64] } } }
            ]
        },
        val:  { type: "string" },
        sign: true,
        pow:  { field: 0, difficulty: 3 },
        params: { baseId, side, candle }
    })
}
