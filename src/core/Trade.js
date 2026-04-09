import { id } from "./Trade/id.js"
import { create } from "./Trade/create.js"
import { deposit } from "./Trade/deposit.js"
import { deliver } from "./Trade/deliver.js"
import { confirm } from "./Trade/confirm.js"
import { dispute } from "./Trade/dispute.js"
import { refund } from "./Trade/refund.js"
import { release } from "./Trade/release.js"
import { read } from "./Trade/read.js"

export class Trade {
    // gun    — GDB instance
    // order  — Order instance (contains orderId, type, item, price, etc.)
    // maker  — { pub, epub, pair? } maker's identity
    // taker  — { pub, epub, pair? } taker's identity
    // escrow — { epub } platform's epub (for DH shared secret)
    constructor({ gun, order, maker, taker, escrow } = {}) {
        if (!gun || !order || !maker || !taker || !escrow) return { error: "invalidInput" }
        this.gun = gun
        this.order = order
        this.maker = maker
        this.taker = taker
        this.escrow = escrow
        this._id = null
    }

    id = id
    create = create
    deposit = deposit
    deliver = deliver
    confirm = confirm
    dispute = dispute
    refund = refund
    release = release
    read = read
}

export default Trade