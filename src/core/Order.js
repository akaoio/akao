import { id } from "./Order/id.js"
import { key } from "./Order/key.js"
import { soul } from "./Order/soul.js"
import { create } from "./Order/create.js"
import { cancel } from "./Order/cancel.js"
import { proof } from "./Order/proof.js"
import { match } from "./Order/match.js"
import { fetch } from "./Order/fetch.js"

export class Order {
    // gun   — GDB instance
    // pair  — maker's SEA pair
    // item  — item slug (e.g. "diablo-4/penitent-greaves-d76bc")
    // type  — "buy" | "sell"
    // price — amount in currency units
    // currency — e.g. "USDT"
    // chain    — EVM chain id
    // referrer — affiliate pub key (optional)
    constructor({ gun, pair, item, type, price, currency, chain, referrer = null } = {}) {
        if (!gun || !pair || !item || !type || !price || !currency || !chain) return { error: "invalidInput" }
        this.gun = gun
        this.pair = pair
        this.item = item
        this.type = type
        this.price = price
        this.currency = currency
        this.chain = chain
        this.referrer = referrer
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