import { soul } from "./soul.js"

// Write order to Gun under the Pen-validated order-book soul.
// Buy orders must call proof() and deposit to FP wallet before create().
export async function create() {
    const orderId = await this.id()
    const k = await this.key()
    const s = soul()
    const payload = JSON.stringify({
        orderId,
        item: this.item,
        type: this.type,
        price: this.price,
        currency: this.currency,
        chain: this.chain,
        referrer: this.referrer,
        status: "open"
    })
    const value = await globalThis.sea.sign(payload, this.pair)
    await this.gun.get(s).get(k).put(value)
    return { orderId, key: k }
}
