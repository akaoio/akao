import { create } from "./create.js"
import { id } from "./id.js"
import { soul } from "./soul.js"
import { Statics } from "/core/Stores.js"

export class Order {
    constructor({ maker, platform, affiliate = null, side, base, quote } = {}) {
        this.maker = maker
        this.platform = platform
        this.affiliate = affiliate
        this.side = side // "buy" or "sell"
        this.base = base // { type: "item", id: "game-item-id", quantity: 1 }
        this.quote = quote // { type: "currency", id: "currency-id", quantity: 100 }
        if (!this.maker || !this.platform || !this.base || !this.quote) throw new Error("invalidInput")
        this.pow = Statics?.system?.pow || { field: 7, difficulty: 3 } // Field 7 (R[7]) is reserved for Proof of Work nonce (["^"] in msg.put)
        this.candle = Statics?.system?.candle || 300000
    }

    id = id

    soul = soul

    create = create
}

export default Order