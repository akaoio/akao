import { Indexes, Statics, Chains, Wallets } from "./Stores.js"
import { NODE, load, merge } from "./Utils.js"
import { loadContract } from "./Utils/contracts.js"
import { V2 } from "./DeFi/V2.js"
import { V3 } from "./DeFi/V3.js"

// Chain Architectures
const Architectures = { V2, V3 }

export default class Dex {
    constructor(configs = {}) {
        this.configs = {}
        Object.assign(this.configs, configs)
        if (!this.configs.dex || !this.configs.version) throw new Error("Dex's configs not found")
        this.pools = {}
        Statics.defis = Statics.defis || {}
        this.init = this.init.bind(this)
        this.load = this.load.bind(this)
    }

    async init(configs = {}) {
        this.configs = this.configs || {}
        const stores = { dex: {}, pools: {} }
        stores.dex.indexes = await Indexes.Statics.get("defis").get(this.id).once()
        stores.dex.statics = Statics.defis[this.id]
        if (!configs || !Object.keys(configs).length) configs = stores.dex.indexes || stores.dex.statics || (await load(["statics", "chains", this.configs.chain, "defis", this.configs.dex, this.configs.version, "configs.json"]))
        merge(this.configs, configs)

        // Load list of pools for this dex, it's an array of addresses only
        if (!this.configs?.pools) {
            stores.pools.indexes = await Indexes.Statics.get("defis").get(this.id).get("pools").once()
            stores.pools.statics = Statics.defis[this.id]?.pools
            const pools = stores.pools.indexes || stores.pools.statics || (await load(["statics", "chains", this.configs.chain, "defis", this.configs.dex, this.configs.version, "pools.json"]))
            merge(this.configs, { pools })
        }

        // Merge configs into global Indexes and Statics.defis
        if (!stores.dex.indexes) Indexes.Statics.get("defis").get(this.id).put(this.configs)
        if (!stores.dex.statics) Statics.defis[this.id] = this.configs || {}

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
