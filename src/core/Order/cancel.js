// Remove order from Gun — maker withdraws listing
// For buy orders: maker should also withdraw FP wallet funds back to main wallet
export async function cancel(key) {
    // TODO: null-put the order key in Gun
    await this.gun.get(this.item).get(key).put(null)
}
