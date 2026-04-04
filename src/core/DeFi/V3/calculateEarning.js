import { BigNumber } from "../../Utils.js"
import { loadContract, loadABI } from "../../Utils/contracts.js"

export const calculateEarning = async function ({ position: _position, token0: _token0, amount0, token1: _token1, amount1 } = {}) {
    try {
        // Get position manager contract
        const positionManager = { configs: await loadContract({ chain: this.configs.chain, address: this.configs.positionManager }) }
        if (!positionManager.configs) throw new Error("Position manager contract not found")
        positionManager.ABI = await loadABI({ ABI: positionManager.configs.ABI, methods: ["positions"] })
        if (!positionManager.ABI) throw new Error("Position manager ABI not found")
        positionManager.contract = this.chain.Contract({ address: this.configs.positionManager, ABI: positionManager.ABI })

        // Get position info
        const position = await positionManager.contract.positions(_position)

        // Get token0 contract
        const token0 = { configs: await loadContract({ chain: this.configs.chain, address: position.token0 }) }
        if (!token0.configs) throw new Error("Token0 contract not found")
        token0.ABI = await loadABI({ ABI: token0.configs.ABI, methods: ["decimals", "balanceOf"] })
        if (!token0.ABI) throw new Error("Token0 ABI not found")
        token0.contract = this.chain.Contract({ address: position.token0, ABI: token0.ABI })
        if (!token0.configs?.decimals) token0.configs.decimals = await token0.contract.decimals()

        // Get token1 contract
        const token1 = { configs: await loadContract({ chain: this.configs.chain, address: position.token1 }) }
        if (!token1.configs) throw new Error("Token1 contract not found")
        token1.ABI = await loadABI({ ABI: token1.configs.ABI, methods: ["decimals", "balanceOf"] })
        if (!token1.ABI) throw new Error("Token1 ABI not found")
        token1.contract = this.chain.Contract({ address: position.token1, ABI: token1.ABI })
        if (!token1.configs?.decimals) token1.configs.decimals = await token1.contract.decimals()

        // Calculate current amounts and fees
        token0.current = new BigNumber(position.tokensOwed0)
        token1.current = new BigNumber(position.tokensOwed1)
        token0.fees = new BigNumber(position.feeGrowthInside0LastX128)
        token1.fees = new BigNumber(position.feeGrowthInside1LastX128)

        // Convert to decimal values
        token0.currentDecimal = token0.current.dividedBy(new BigNumber(10).pow(token0.configs.decimals))
        token1.currentDecimal = token1.current.dividedBy(new BigNumber(10).pow(token1.configs.decimals))
        token0.feesDecimal = token0.fees.dividedBy(new BigNumber(10).pow(token0.configs.decimals))
        token1.feesDecimal = token1.fees.dividedBy(new BigNumber(10).pow(token1.configs.decimals))

        // Calculate total values
        token0.total = token0.currentDecimal.plus(token0.feesDecimal)
        token1.total = token1.currentDecimal.plus(token1.feesDecimal)

        // Calculate earnings and percentages
        token0.earning = token0.total.minus(amount0)
        token1.earning = token1.total.minus(amount1)
        token0.percentage = amount0 ? token0.earning.dividedBy(amount0).multipliedBy(100) : new BigNumber(0)
        token1.percentage = amount1 ? token1.earning.dividedBy(amount1).multipliedBy(100) : new BigNumber(0)

        return {
            token0: {
                amount: amount0,
                current: token0.currentDecimal.toString(),
                fees: token0.feesDecimal.toString(),
                total: token0.total.toString(),
                earning: token0.earning.toString(),
                percentage: token0.percentage.toString()
            },
            token1: {
                amount: amount1,
                current: token1.currentDecimal.toString(),
                fees: token1.feesDecimal.toString(),
                total: token1.total.toString(),
                earning: token1.earning.toString(),
                percentage: token1.percentage.toString()
            }
        }
    } catch (error) {
        return { error: error.message }
    }
}

export default calculateEarning
