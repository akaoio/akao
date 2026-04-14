import { id } from "./Order/id.js"
import { key } from "./Order/key.js"
import { soul } from "./Order/soul.js"
import { create } from "./Order/create.js"
import { cancel } from "./Order/cancel.js"
import { proof } from "./Order/proof.js"
import { match } from "./Order/match.js"
import { fetch } from "./Order/fetch.js"
import { normalize, validate } from "./Order/schema.js"

export class Order {
    // pair     — maker's SEA pair
    // side     — "buy" | "sell"
    // base     — { type: "item", id, quantity }
    // quote    — { type: "crypto", quantity, contract, chain }
    // affiliate — { pub } | pub string (optional)
    // xpub     — maker's root extended public key (required in every raw order maker object)
    constructor({ pair, side, base, quote, affiliate = null, referrer = null, xpub = null } = {}) {
        if (!pair) throw new Error("invalidInput")
        const normalized = normalize({ pair, side, base, quote, affiliate, referrer, xpub })
        validate(normalized)
        this.pair = pair
        this.side = normalized.side
        this.maker = normalized.maker
        this.base = normalized.base
        this.quote = normalized.quote
        this.affiliate = normalized.affiliate
    }

    id = id
    key = key
    soul = soul
    create = create
    cancel = cancel
    proof = proof
    match = match

    static fetch = fetch
}

export default Order
