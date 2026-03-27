import { BigNumber, loadContract, loadABI } from "../../Utils.js"

export const getRate = async function ({ pool: _pool, block } = {}) {
    try {
        // Get pool contract to get reserves
        const pool = { configs: await loadContract({ chain: this.configs.chain, address: _pool }) }
        if (!pool.configs) throw new Error("Pool contract not found")
        pool.ABI = await loadABI({ ABI: pool.configs.ABI, methods: ["getReserves"] })
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

        // Get reserves and decimals
        // If block is provided, get reserves at that block
        const reserves = block ? await pool.contract.getReserves({ blockTag: Number(block) }) : await pool.contract.getReserves()
        if (!reserves || !reserves._reserve0 || !reserves._reserve1) 
            return {
                token0: { quantity: 0, rate: 0 },
                token1: { quantity: 0, rate: 0 }
            }
        

        // Convert reserves to BigNumber
        token0.reserve = new BigNumber(reserves._reserve0) // token0 reserve
        token1.reserve = new BigNumber(reserves._reserve1) // token1 reserve

        // If either reserve is 0, return 0 values
        if (token0.reserve.isZero() || token1.reserve.isZero()) 
            return {
                token0: { quantity: 0, rate: 0 },
                token1: { quantity: 0, rate: 0 }
            }
        

        // Convert reserves to decimal values
        token0.reserveDecimal = token0.reserve.dividedBy(new BigNumber(10).pow(token0.configs?.decimals))
        token1.reserveDecimal = token1.reserve.dividedBy(new BigNumber(10).pow(token1.configs?.decimals))

        // Calculate prices:
        // token0.rate = how many token1 you get for 1 token0 = reserve1/reserve0
        // token1.rate = how many token0 you get for 1 token1 = reserve0/reserve1
        token0.rate = token1.reserveDecimal.dividedBy(token0.reserveDecimal) // How many token1 per token0
        token1.rate = token0.reserveDecimal.dividedBy(token1.reserveDecimal) // How many token0 per token1

        return {
            token0: {
                quantity: token0.reserveDecimal.toNumber(),
                rate: token0.rate.toNumber() // How many token1 per token0
            },
            token1: {
                quantity: token1.reserveDecimal.toNumber(),
                rate: token1.rate.toNumber() // How many token0 per token1
            }
        }
    } catch (error) {
        console.error("V2 error:", error)
        return { error: error.message }
    }
}

export default getRate
