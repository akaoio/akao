import { soul } from "./soul.js"

// Remove order from Gun — maker withdraws listing.
// For buy orders: maker should also withdraw FP wallet funds back to main wallet.
export async function cancel(key) {
    await this.gun.get(soul()).get(key).put(null)
}
