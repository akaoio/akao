import { soul } from "./soul.js"
import { parts } from "./parts.js"
import { payload } from "./schema.js"
import zen from "../../src/core/ZEN.js"

// Write order to Gun under the Pen-validated market-window soul.
// The raw order value is a normalized signed intent with maker/base/quote objects.
export async function create() {
    const orderId = await this.id()
    const k = await this.key()
    const meta = parts(k)
    if (!meta) throw new Error("invalidKey")
    const s = soul.call(this, { candle: meta.candle })
    const data = payload(this, { orderId, status: "open" })
    const value = await zen.sign(JSON.stringify(data), this.pair)
    await new Promise(r => zen.get(s).get(k).put(value, r, { authenticator: this.pair }))
    return { orderId, key: k }
}
