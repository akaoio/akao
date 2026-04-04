import { BigNumber } from "../../Utils.js"
import { loadContract, loadABI } from "../../Utils/contracts.js"

export const calculateEarning = async function ({ position: _position, token0: _token0, amount0, token1: _token1, amount1 } = {}) {
    try {
        const position = { configs: await loadContract({ chain: this.configs.chain, address: _position }) }
        if (!position.configs) throw new Error("position contract not found")
        position.ABI = await loadABI({ ABI: position.configs.ABI, methods: ["decimals", "balanceOf", "totalSupply"] })
        if (!position.ABI) throw new Error("Position ABI not found")
        position.contract = this.chain.Contract({ address: _position, ABI: position.ABI })

        const token0 = { configs: await loadContract({ chain: this.configs.chain, address: _token0 }) }
        if (!token0.configs) throw new Error("Token0 contract not found")
        token0.ABI = await loadABI({ ABI: token0.configs.ABI, methods: ["decimals", "balanceOf"] })
        if (!token0.ABI) throw new Error("Token0 ABI not found")
        token0.contract = this.chain.Contract({ address: _token0, ABI: token0.ABI })
        if (!token0.configs?.decimals) token0.configs.decimals = await token0.contract.decimals()

        const token1 = { configs: await loadContract({ chain: this.configs.chain, address: _token1 }) }
        if (!token1.configs) throw new Error("Token1 contract not found")
        token1.ABI = await loadABI({ ABI: token1.configs.ABI, methods: ["decimals", "balanceOf"] })
        if (!token1.configs) throw new Error("Token1 ABI not found")
        token1.contract = this.chain.Contract({ address: _token1, ABI: token1.ABI })
        if (!token1.configs?.decimals) token1.configs.decimals = await token1.contract.decimals()

        // Get current position balance
        position.balance = await position.contract.balanceOf(this.wallet.address)
        const totalSupply = await position.contract.totalSupply()

        // Get current reserves
        token0.reserve = await token0.contract.balanceOf(_position)
        token1.reserve = await token1.contract.balanceOf(_position)

        // Calculate current amounts based on position share
        token0.current = new BigNumber(token0.reserve).multipliedBy(position.balance).dividedBy(totalSupply)
        token1.current = new BigNumber(token1.reserve).multipliedBy(position.balance).dividedBy(totalSupply)

        token0.currentDecimal = token0.current.dividedBy(new BigNumber(10).pow(token0.configs?.decimals)).toNumber()
        token1.currentDecimal = token1.current.dividedBy(new BigNumber(10).pow(token1.configs?.decimals)).toNumber()

        // Calculate earning and percentage
        token0.earning = new BigNumber(token0.currentDecimal).minus(amount0).toNumber()
        token1.earning = new BigNumber(token1.currentDecimal).minus(amount1).toNumber()
        token0.percentage = new BigNumber(token0.earning).dividedBy(amount0).multipliedBy(100).toNumber()
        token1.percentage = new BigNumber(token1.earning).dividedBy(amount1).multipliedBy(100).toNumber()

        return {
            token0: {
                amount: amount0,
                current: token0.current,
                earning: token0.earning,
                percentage: token0.percentage
            },
            token1: {
                amount: amount1,
                current: token1.current,
                earning: token1.earning,
                percentage: token1.percentage
            }
        }
    } catch (error) {
        return { error: error.message }
    }
}

export default calculateEarning
