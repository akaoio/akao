import { soul } from "./soul.js"

// Remove order from Gun — maker withdraws listing.
// For buy orders: maker should also withdraw FP wallet funds back to main wallet.
// Ownership is verified by checking pub8 in key segment 3 matches this.pair.pub.
export async function cancel(key) {
    const segs = key.split(":")
    if (!segs[3] || segs[3] !== this.pair.pub.slice(0, 8)) return { error: "notOwner" }
    await this.gun.get(soul()).get(key).put(null, null, { opt: { authenticator: this.pair } })
}
