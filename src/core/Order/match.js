import { sha256 } from "../Utils/crypto.js"
import { soul } from "./soul.js"
import { parts } from "./parts.js"

// Taker accepts order — computes deterministic tradeId and writes matched status.
// tradeId = sha256("TR:" + orderId + ":" + makerpub + ":" + takerpub)
// No timestamp in tradeId — same inputs always produce same tradeId.
// Gun HAM (last-write-wins by state) handles simultaneous double-match:
// both takers write "matched"; whichever has lower HAM timestamp loses.
// Callers must re-read the order after match to confirm their write won.
export async function match({ orderId, makerpub, key }) {
    const tradeId = sha256("TR:" + orderId + ":" + makerpub + ":" + this.pair.pub)
    if (key) {
        const meta = parts(key)
        if (!meta) return { error: "invalidKey" }
        const data = JSON.stringify({ tradeId, taker: this.pair.pub, status: "matched" })
        const value = await globalThis.sea.sign(data, this.pair)
        await new Promise(r => this.gun.get(soul.call(this, { candle: meta.candle })).get(key).put(value, r, { opt: { authenticator: this.pair } }))
    }
    return { tradeId }
}
