// Returns the Pen soul string for the global order-book namespace.
// All orders live under this one soul — keyed by candle:item:type:pub8:nonce.
//
// Pen enforces per-write:
//   key  — seg 0: candle window (±100 back / 2 fwd, 5-min size)
//           seg 1: item slug  1–128 chars
//           seg 2: type       "buy" | "sell"
//           seg 3: pub8       exactly 8 chars
//   val  — must be a string (SEA-signed JSON)
//   sign — writer must be authenticated (enforces identity via R[5])
//   pow  — SHA-256(key) starts with "00" (difficulty 2)
//
// Note: PRE(R[5], seg3) is not expressible in Pen DSL — `pre` only accepts
// string literals. `sign: true` already binds the write to the holder of the
// private key in R[5]. pub8 in the key is a LEX-query aid only.
//
// The soul is deterministic (static spec) — same value every call.
export function soul() {
    const { pen, candle } = globalThis.sea
    return pen({
        key: {
            and: [
                candle({ seg: 0, sep: ':', size: 300000, back: 100, fwd: 2 }),
                { seg: { sep: ':', idx: 1, of: { reg: 0 }, match: { length: [1, 128] } } },
                { seg: { sep: ':', idx: 2, of: { reg: 0 }, match: { or: [{ eq: 'buy' }, { eq: 'sell' }] } } },
                { seg: { sep: ':', idx: 3, of: { reg: 0 }, match: { length: [8, 8] } } }
            ]
        },
        val:  { type: 'string' },
        sign: true,
        pow:  { field: 0, difficulty: 2 }
    })
}
