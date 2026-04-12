import { soul } from "./soul.js"
import { parts } from "./parts.js"

// Remove order from Gun — maker withdraws listing.
// For buy orders: maker should also withdraw FP wallet funds back to main wallet.
// Ownership is verified by checking full pub in key segment 1 matches this.pair.pub.
export async function cancel(key) {
    const meta = parts(key)
    if (!meta || meta.pub !== this.pair.pub) return { error: "notOwner" }
    await new Promise(r => this.gun.get(soul.call(this, { candle: meta.candle })).get(key).put(null, r, { opt: { authenticator: this.pair } }))
}
