import { loadContract, loadABI } from "../../Utils/contracts.js"

export const pool = async function ({ token0, token1, fee = 3000 } = {}) {
    const factory = { configs: await loadContract({ chain: this.configs.chain, address: this.configs.factory }) }
    if (!factory.configs) throw new Error("Factory contract not found")
    factory.ABI = await loadABI({ ABI: factory.configs.ABI, methods: ["getPool"] })
    if (!factory.ABI) throw new Error("Factory ABI not found")
    factory.contract = this.chain.Contract({ address: this.configs.factory, ABI: factory.ABI })

    const pool = await factory.contract.getPool(token0, token1, fee)
    if (pool == "0x0000000000000000000000000000000000000000") return

    return {
        address: pool,
        token0,
        token1,
        fee
    }
}

export default pool
