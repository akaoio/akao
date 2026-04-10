import { soul } from "./soul.js"

// Write order to Gun under the Pen-validated order-book soul.
// Buy orders must call proof() and deposit to FP wallet before create().
// xpub is included in buy order payloads so Takers can verify FP wallet on-chain.
export async function create() {
    const orderId = await this.id()
    const k = await this.key()
    const s = soul()
    const data = {
        orderId,
        pub: this.pair.pub,
        item: this.item,
        type: this.type,
        price: this.price,
        currency: this.currency,
        chain: this.chain,
        referrer: this.referrer,
        status: "open"
    }
    if (this.type === "buy" && this.xpub) data.xpub = this.xpub
    const value = await globalThis.sea.sign(JSON.stringify(data), this.pair)
    await this.gun.get(s).get(k).put(value, null, { opt: { authenticator: this.pair } })
    return { orderId, key: k }
}
