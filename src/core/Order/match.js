import { sha256 } from "../Utils/crypto.js"

// Taker accepts order — computes tradeId once and stores in Gun
// tradeId must be computed here, stored in Gun, never recomputed elsewhere
// "TR:" domain separator prevents hash space collision with orderId ("OR:")
export async function match({ orderId, makerpub }) {
    const ts = Date.now()
    const tradeId = sha256("TR:" + orderId + ":" + makerpub + ":" + this.pair.pub + ":" + ts)
    // TODO: write matched status to Gun trade record (both namespaces)
    // TODO: update order status to "matched"
    return { tradeId, ts }
}
