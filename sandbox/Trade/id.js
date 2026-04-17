import { sha256 } from "../../src/core/Utils/crypto.js"
import { resolveOrderId } from "./helpers.js"

// tradeId must match Order.match():
// sha256("TR:" + orderId + ":" + makerpub + ":" + takerpub)
// It is cached per trade instance once resolved.
export async function id({ orderId } = {}) {
    if (this._id) return this._id
    const resolvedOrderId = await resolveOrderId(this.order, orderId)
    this._id = sha256("TR:" + resolvedOrderId + ":" + this.maker.pub + ":" + this.taker.pub)
    return this._id
}
