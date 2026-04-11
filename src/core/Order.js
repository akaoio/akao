import { id } from "./Order/id.js"
import { key } from "./Order/key.js"
import { soul } from "./Order/soul.js"
import { create } from "./Order/create.js"
import { cancel } from "./Order/cancel.js"
import { proof } from "./Order/proof.js"
import { match } from "./Order/match.js"
import { fetch } from "./Order/fetch.js"

export class Order {
    // gun      — GDB instance
    // pair     — maker's SEA pair
    // item     — item slug (e.g. "diablo-4/penitent-greaves-d76bc"), must not contain ":"
    // type     — "buy" | "sell"
    // price    — amount in currency units (must be > 0)
    // currency — e.g. "USDT"
    // chain    — EVM chain id
    // referrer — affiliate pub key (optional)
    // xpub     — maker's root extended public key (required for buy orders — fund proof)
    constructor({ gun, pair, item, type, price, currency, chain, referrer = null, xpub = null } = {}) {
        if (!gun || !pair || !item || !type || price === undefined || price === null || !currency || !chain) throw new Error("invalidInput")
        if (typeof price !== "number" || !isFinite(price) || price <= 0) throw new Error("invalidPrice")
        if (type !== "buy" && type !== "sell") throw new Error("invalidType")
        if (item.includes(":")) throw new Error("invalidItem")
        if (type === "buy" && !xpub) throw new Error("xpubRequired")
        this.gun = gun
        this.pair = pair
        this.item = item
        this.type = type
        this.price = price
        this.currency = currency
        this.chain = chain
        this.referrer = referrer
        this.xpub = xpub
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