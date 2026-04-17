import { secret } from "./Lock/secret.js"
import { index } from "./Lock/index.js"
import { address } from "./Lock/address.js"
import { unlock } from "./Lock/unlock.js"

export class Lock {
    constructor({ payer, platform, recipient, tradeId, trade, type = "TL" } = {}) {
        if (!payer || !platform || !recipient || !(tradeId || trade)) throw new Error("invalidInput")
        this.payer = payer
        this.platform = platform
        this.recipient = recipient
        this.tradeId = tradeId || trade
        this.type = type
    }

    secret = secret
    index = index
    address = address
    unlock = unlock
}
