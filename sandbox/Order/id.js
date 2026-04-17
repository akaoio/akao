import { sha256 } from "../../src/core/Utils/crypto.js"
import { canonical } from "./schema.js"

// orderId is computed once per Order instance (cached _ts + _rnd prevent drift and collisions).
// Random 8 bytes (_rnd) guard against timestamp collisions when same maker creates
// two identical intents within the same millisecond.
// "OR:" domain separator isolates order hash space from trade ("TR:") and FP ("FP:")
export async function id() {
    this._ts = this._ts || Date.now()
    this._rnd = this._rnd || Array.from(
        crypto.getRandomValues(new Uint8Array(8)),
        b => b.toString(16).padStart(2, "0")
    ).join("")
    return sha256("OR:" + canonical(this) + ":" + this._ts + ":" + this._rnd)
}
