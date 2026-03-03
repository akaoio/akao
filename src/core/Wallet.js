import { Chains } from "./Stores.js"
import { merge } from "./Utils/data.js"
import { sha256 } from "./Utils/crypto.js"
import { Access } from "./Access.js"

export default class Wallet {
    constructor(configs = {}) {
        this.configs = {}
        Object.assign(this.configs, configs)
        this.init = this.init.bind(this)
        this.balance = this.balance.bind(this)
        this.send = this.send.bind(this)
    }

    async init(configs = {}) {
        this.configs = this.configs || {}
        if (!this.configs.chain) throw new Error("Chain is required")
        merge(this.configs, configs)
    }

    get id() {
        return this.configs.chain
    }

    get chain() {
        return Chains[this.configs.chain]
    }

    get seed() {
        const { sea } = globalThis
        if (!Access.get("authenticated") || !Access.get("pair")?.priv || Access.get("wallet")?.id == null || !sea) return
        const id = Access.get("wallet").id
        const priv = Access.get("pair").priv
        const seed = sha256(priv + id)
        return seed
    }

    get private() {
        if (!this.seed) return
        return this.chain.private(this.seed)
    }

    get public() {
        if (!this.private) return
        return this.chain.public(this.private)
    }

    get address() {
        return this.public
    }

    async balance({ currency }) {
        return await this.chain.balance({ address: this.address, currency })
    }

    async send({ to, amount, currency, callback }) {
        const tx = await this.chain.send({ from: this.address, to, amount, currency })
        if (tx.error) console.error(tx.error)
        if (callback && typeof callback === "function") callback(tx)
        return tx
    }
}

export { Wallet }
