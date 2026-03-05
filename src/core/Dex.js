import { Chains, Wallets } from "./Stores.js"
import { merge } from "./Utils.js"
import { loadContract } from "./Utils/contracts.js"
import { V2 } from "./DeFi/V2.js"
import { V3 } from "./DeFi/V3.js"
import DB from "./DB.js"

// Chain Architectures
const Architectures = { V2, V3 }

export default class Dex {
    constructor(configs = {}) {
        this.configs = {}
        Object.assign(this.configs, configs)
        if (!this.configs.dex || !this.configs.version) throw new Error("Dex's configs not found")
        this.pools = {}
        this.init = this.init.bind(this)
        this.load = this.load.bind(this)
    }

    async init(configs = {}) {
        this.configs = this.configs || {}
        if (!configs || !Object.keys(configs).length) configs = await DB.get(["statics", "chains", this.configs.chain, "defis", this.configs.dex, this.configs.version, "configs.json"])
        merge(this.configs, configs)

        // Load list of pools for this dex, it's an array of addresses only
        if (!this.configs?.pools) {
            const pools = await DB.get(["statics", "chains", this.configs.chain, "defis", this.configs.dex, this.configs.version, "pools.json"])
            merge(this.configs, { pools })
        }

        // Load architecture
        if (Architectures[this.configs?.version]) Object.assign(this, Architectures[this.configs.version])

        // Initialize architecture
        if (typeof this.construct === "function") this.construct()
    }

    async load({ address } = {}) {
        if (!address) return
        const contract = await loadContract({ chain: this.configs.chain, address })
        if (!contract) return
        this.pools[address] = contract
    }

    get name() {
        return `${this.configs.dex}${this.configs.version}`
    }

    get id() {
        return `${this.configs.chain}.${this.name}`
    }

    get chain() {
        if (!Chains[this.configs.chain]) return
        return Chains[this.configs.chain]
    }

    get wallet() {
        if (!Wallets[this.configs.chain]) return
        return Wallets[this.configs.chain]
    }

    get https() {
        return this.chain.https
    }

    get wss() {
        return this.chain.wss
    }
}

export { Dex }
