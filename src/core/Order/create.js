// Write order to Gun with Pen-validated key
// Buy orders must call proof() and deposit to FP wallet before create()
export async function create() {
    const orderId = await this.id()
    const k = await this.key()
    const value = JSON.stringify({
        orderId,
        item: this.item,
        type: this.type,
        price: this.price,
        currency: this.currency,
        chain: this.chain,
        referrer: this.referrer,
        status: "open"
    })
    // TODO: determine orderSoul from item
    await this.gun.get(this.item).get(k).put(value)
    return { orderId, key: k }
}
