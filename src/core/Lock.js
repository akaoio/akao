import { secret } from "./Lock/secret.js"
import { index } from "./Lock/index.js"
import { address } from "./Lock/address.js"
import { unlock } from "./Lock/unlock.js"

export class Lock {
    constructor({ payer, escrow, recipient, trade, type = "TL" } = {}) {
        if (!payer || !escrow || !recipient || !trade) return { error: "invalidInput" }
        this.payer = payer
        this.escrow = escrow
        this.recipient = recipient
        this.trade = trade
        this.type = type
    }

    secret = secret
    index = index
    address = address
    unlock = unlock
}