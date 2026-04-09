import { sha256 } from "../Utils/crypto.js"

// tradeId is computed ONCE at match time — stored in Gun, never recomputed
// Caller must pass the exact timestamp used at match time
// "TR:" domain separator prevents hash space collision with orderId ("OR:")
export async function id({ orderId, ts }) {
    if (this._id) return this._id
    this._id = sha256("TR:" + orderId + ":" + this.maker.pub + ":" + this.taker.pub + ":" + ts)
    return this._id
}
