// DRAFT
import { create } from "./create.js"
import { id } from "./id.js"
import { soul } from "./soul.js"

export class Order {
    constructor({ maker, platform, affiliate = null, side, base, quote } = {}) {
        this.maker = maker
        this.platform = platform
        this.affiliate = affiliate
        this.side = side // "buy" or "sell"
        this.base = base // { type: "item", id: "game-item-id", quantity: 1 }
        this.quote = quote // { type: "currency", id: "currency-id", quantity: 100 }
        if (!this.maker || !this.platform || !this.base || !this.quote) throw new Error("invalidInput")
    }

    id = id

    soul = soul

    create = create
}

export default Order