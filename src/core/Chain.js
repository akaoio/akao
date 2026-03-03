import { Indexes, Statics, Wallets } from "./Stores.js"
import { NODE, load, merge } from "./Utils.js"
import { loadContract } from "./Utils/contracts.js"
import { EVM } from "./Chains/EVM.js"

// Chain Architectures
const Architectures = { EVM }

export default class Chain {
    constructor(configs = {}) {
        this.configs = {}
        Object.assign(this.configs, configs)
        this._https = undefined // Currently active https connection
        this._httpsURL = undefined // Currently active https url
        this._wss = undefined // Currently active wss connection
        this._wssURL = undefined // Currently active wss url
        this.blocks = {}
        this.currencies = {}
        this.stables = {}
        Statics.chains = Statics.chains || {}
        this.init = this.init.bind(this)
        this.load = this.load.bind(this)
        this.reconnect = this.reconnect.bind(this)
        this.watch = this.watch.bind(this)
        this.Contract = this.Contract.bind(this)
    }

    async init(configs = {}) {
        this.configs = this.configs || {}
        const stores = { chain: {}, currencies: {}, stables: {} }
        stores.chain.indexes = await Indexes.Statics.get("chains").get(this.id).once()
        stores.chain.statics = Statics.chains[this.id]
        if (!configs || !Object.keys(configs).length) configs = stores.chain.indexes || stores.chain.statics || (await load(["statics", "chains", this.id, "configs.json"]))
        merge(this.configs, configs)

        // Load list of currencies for this chain, it's an array of addresses only
        if (!this.configs?.currencies) {
            stores.currencies.indexes = await Indexes.Statics.get("chains").get(this.id).get("currencies").once()
            stores.currencies.statics = Statics.chains[this.id]?.currencies
            const currencies = stores.currencies.indexes || stores.currencies.statics || (await load(["statics", "chains", this.id, "currencies.json"]))
            merge(this.configs, { currencies })
        }

        // Load precalculated stables
        if (!this.configs?.stables) {
            stores.stables.indexes = await Indexes.Statics.get("chains").get(this.id).get("stables").once()
            stores.stables.statics = Statics.chains[this.id]?.stables
            const stables = stores.stables.indexes || stores.stables.statics || (await load(["statics", "chains", this.id, "stables.json"]))
            merge(this.configs, { stables })
        }

        await this.load({ address: "native" }) // Load native currency

        // Merge configs into global Indexes and Statics.chains
        if (!stores.chain.indexes) Indexes.Statics.get("chains").get(this.id).put(this.configs)
        if (!stores.chain.statics) Statics.chains[this.id] = this.configs || {}

        // Load architecture
        if (Architectures[this.configs?.architecture]) Object.assign(this, Architectures[configs?.architecture])

        if (typeof this.connect === "undefined") throw new Error(`No connection function found for chain ${this.id}`)

        // Initialize architecture
        if (typeof this.construct === "function") this.construct()
    }

    async load({ address } = {}) {
        if (!address) return
        const contract = await loadContract({ chain: this.id, address })
        if (!contract || contract.type !== "currency") return
        this.currencies[address] = contract
        if (!this.configs.stables.includes(address)) this.stables[address] = contract
    }

    get https() {
        // For https connections, we can create new random connections to reduce rate limiting
        this.configs.https = this.configs?.RPCs?.filter?.((rpc) => rpc.startsWith("http")) || []
        if (!this.configs?.https?.length) return
        this._httpsURL = this.configs.https[Math.floor(Math.random() * this.configs.https.length)]
        if (!this._httpsURL) throw new Error(`HTTP RPC not found for chain ${this.id}`)
        this._https = this.connect(this._httpsURL)
        return this._https
    }

    get wss() {
        // For websocket connections, we need to reuse the same connection
        if (this._wss) return this._wss
        this.configs.wss = this.configs?.RPCs?.filter?.((rpc) => rpc.startsWith("ws")) || []
        if (!this.configs?.wss?.length) return
        this._wssURL = this.configs.wss[Math.floor(Math.random() * this.configs.wss.length)]
        if (!this._wssURL) throw new Error(`WebSocket RPC not found for chain ${this.id}`)
        this._wss = this.connect(this._wssURL)
        return this._wss
    }

    async reconnect() {
        if (this._reconnect > 5) return
        this._reconnect = (this._reconnect || 0) + 1
        setTimeout(() => {
            console.log("Attempting WebSocket reconnection...")
            if (!this.configs?.wss?.length) return
            if (this._wss?.destroy) {
                this._wss.destroy()
                this._wss = undefined
            }
            const options = this.configs.wss.filter((rpc) => rpc !== this._wssURL)
            const wss = options[Math.floor(Math.random() * options.length)]
            if (!wss) throw new Error(`WebSocket RPC not found for reconnecting chain ${this.id}`)
            this._wss = this.connect(wss)
            return this._wss
        }, 5000) // Retry after 5 seconds
    }

    get id() {
        return this.configs?.id
    }

    // Watch blocks
    watch(block) {
        this.blocks[block.number] = block
        // Remove the oldest block if we exceed 10
        while (Object.keys(this.blocks).length > 10) {
            const oldest = Math.min(...Object.keys(this.blocks).map(Number))
            delete this.blocks[oldest]
        }
    }

    // Get the latest block
    get block() {
        if (!this.blocks || Object.keys(this.blocks).length === 0) return
        const latestKey = Math.max(...Object.keys(this.blocks).map(Number))
        return this.blocks[latestKey]
    }

    // Get the average block time
    get duration() {
        if (!this.blocks || Object.keys(this.blocks).length < 2) return
        let total = 0,
            count = 0,
            last = null
        for (const block of Object.values(this.blocks)) {
            if (last !== null) {
                total += Number(block.timestamp) - last
                count++
            }
            last = Number(block.timestamp)
        }
        return Math.floor(total / count)
    }

    // Return a new contract instance
    Contract({ address, ABI } = {}) {
        return this._Contract({ address, ABI })
    }

    get wallet() {
        if (!Wallets[this.id]) return
        return Wallets[this.id]
    }
}

export { Chain }
