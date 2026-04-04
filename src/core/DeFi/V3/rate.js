import { BigNumber } from "../../Utils.js"
import { loadContract, loadABI } from "../../Utils/contracts.js"

export const rate = async function ({ pool: _pool, block } = {}) {
    try {
        // Get pool contract
        const pool = { configs: await loadContract({ chain: this.configs.chain, address: _pool }) }
        if (!pool.configs) throw new Error("Pool contract not found")
        pool.ABI = await loadABI({ ABI: pool.configs.ABI, methods: ["slot0", "liquidity"] })
        if (!pool.ABI) throw new Error("Pool ABI not found")
        pool.contract = this.chain.Contract({ address: _pool, ABI: pool.ABI })

        // Get token0 contract
        const token0 = { configs: await loadContract({ chain: this.configs.chain, address: pool.configs.token0 }) }
        if (!token0.configs) throw new Error("Token0 contract not found")
        if (!token0.configs?.decimals) {
            token0.ABI = await loadABI({ ABI: token0.configs.ABI, methods: ["decimals"] })
            if (!token0.ABI) throw new Error("Token0 ABI not found")
            token0.contract = this.chain.Contract({ address: pool.configs.token0, ABI: token0.ABI })
            token0.configs.decimals = await token0.contract.decimals()
        }

        // Get token1 contract
        const token1 = { configs: await loadContract({ chain: this.configs.chain, address: pool.configs.token1 }) }
        if (!token1.configs) throw new Error("Token1 contract not found")
        if (!token1.configs?.decimals) {
            token1.ABI = await loadABI({ ABI: token1.configs.ABI, methods: ["decimals"] })
            if (!token1.ABI) throw new Error("Token1 ABI not found")
            token1.contract = this.chain.Contract({ address: pool.configs.token1, ABI: token1.ABI })
            token1.configs.decimals = await token1.contract.decimals()
        }

        // Get liquidity and slot0 data
        // If block is provided, get data at that block
        const [liquidity, slot0] = await Promise.all([block ? pool.contract.liquidity({ blockTag: Number(block) }) : pool.contract.liquidity(), block ? pool.contract.slot0({ blockTag: Number(block) }) : pool.contract.slot0()])

        // Check for zero or invalid values
        if (!liquidity || !slot0 || !slot0.sqrtPriceX96 || liquidity === "0") 
            return {
                token0: { quantity: 0, rate: 0 },
                token1: { quantity: 0, rate: 0 }
            }
        

        // In Uniswap V3, sqrtPriceX96 represents √(token1/token0) * 2^96
        const sqrtPriceX96 = new BigNumber(slot0.sqrtPriceX96)
        const sqrtPrice = sqrtPriceX96.dividedBy(new BigNumber(2).pow(96))

        // Calculate price = (sqrtPrice)^2 = token1/token0
        const price = sqrtPrice.multipliedBy(sqrtPrice)

        // Adjust for decimals
        const decimalAdjustment = new BigNumber(10).pow(token0.configs.decimals - token1.configs.decimals)
        const adjustedPrice = price.multipliedBy(decimalAdjustment)

        // Calculate quantities
        const L = new BigNumber(liquidity)
        token0.quantity = L.dividedBy(sqrtPrice).dividedBy(new BigNumber(10).pow(token0.configs.decimals)).toNumber()

        token1.quantity = L.multipliedBy(sqrtPrice).dividedBy(new BigNumber(10).pow(token1.configs.decimals)).toNumber()

        // Final rates depend on token order in pool
        token0.rate = adjustedPrice.toNumber() // How many token1 per token0
        token1.rate = 1 / token0.rate // How many token0 per token1

        return {
            token0: {
                quantity: token0.quantity,
                rate: token0.rate // How many token1 per token0
            },
            token1: {
                quantity: token1.quantity,
                rate: token1.rate // How many token0 per token1
            }
        }
    } catch (error) {
        console.error("V3 error:", error)
        return { error: error.message }
    }
}

export default rate
