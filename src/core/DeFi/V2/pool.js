import { loadContract, loadABI } from "../../Utils/contracts.js"

export const pool = async function ({ token0, token1 } = {}) {
    const configs = await loadContract({ chain: this.configs.chain, address: this.configs.factory })
    if (!configs) throw new Error("Factory contract not found")
    const ABI = await loadABI({ ABI: configs.ABI, methods: ["getPair"] })
    if (!ABI) throw new Error("Factory ABI not found")
    const contract = this.chain.Contract({ address: this.configs.factory, ABI })
    const pool = await contract.getPair(token0, token1)
    if (pool == "0x0000000000000000000000000000000000000000") return
    return {
        address: pool,
        token0,
        token1
    }
}

export default pool
