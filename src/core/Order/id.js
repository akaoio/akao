import { sha256 } from "../Utils/crypto.js"

// orderId is computed once at order creation time and stored in Gun value
// "OR:" domain separator isolates order hash space from trade ("TR:") and FP ("FP:")
export async function id() {
    const ts = Date.now()
    this._ts = this._ts || ts
    return sha256("OR:" + this.pair.pub + ":" + this.item + ":" + this.price + ":" + this._ts)
}
