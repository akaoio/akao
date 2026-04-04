import { BigNumber } from "../../Utils.js"
import { loadContract, loadABI } from "../../Utils/contracts.js"

// Max uint128 as BigNumber
const MAX_UINT128 = new BigNumber(2).pow(128).minus(1)

export const remove = async function ({ position: _position, amount, slippage = 0.5 } = {}) {
    if (!this.wallet) throw new Error("Wallet not connected")
    try {
        // Get position manager contract
        const positionManager = { configs: await loadContract({ chain: this.configs.chain, address: this.configs.positionManager }) }
        if (!positionManager.configs) throw new Error("Position manager contract not found")
        positionManager.ABI = await loadABI({ ABI: positionManager.configs.ABI, methods: ["positions", "decreaseLiquidity", "collect"] })
        if (!positionManager.ABI) throw new Error("Position manager ABI not found")
        positionManager.contract = this.chain.Contract({ address: this.configs.positionManager, ABI: positionManager.ABI })

        // Get position info
        const position = await positionManager.contract.positions(_position)

        // Get token0 contract
        const token0 = { configs: await loadContract({ chain: this.configs.chain, address: position.token0 }) }
        if (!token0.configs) throw new Error("Token0 contract not found")
        token0.ABI = await loadABI({ ABI: token0.configs.ABI, methods: ["decimals"] })
        if (!token0.ABI) throw new Error("Token0 ABI not found")
        token0.contract = this.chain.Contract({ address: position.token0, ABI: token0.ABI })
        if (!token0.configs?.decimals) token0.configs.decimals = await token0.contract.decimals()

        // Get token1 contract
        const token1 = { configs: await loadContract({ chain: this.configs.chain, address: position.token1 }) }
        if (!token1.configs) throw new Error("Token1 contract not found")
        token1.ABI = await loadABI({ ABI: token1.configs.ABI, methods: ["decimals"] })
        if (!token1.ABI) throw new Error("Token1 ABI not found")
        token1.contract = this.chain.Contract({ address: position.token1, ABI: token1.ABI })
        if (!token1.configs?.decimals) token1.configs.decimals = await token1.contract.decimals()

        // Calculate liquidity amount to remove
        const liquidityToRemove = amount ? new BigNumber(position.liquidity).multipliedBy(amount).dividedBy(100).integerValue().toString(10) : position.liquidity // If no amount specified, remove all liquidity

        // Calculate expected output amounts based on current pool state
        const [amount0Expected, amount1Expected] = await positionManager.contract.decreaseLiquidity({
            tokenId: _position,
            liquidity: liquidityToRemove,
            amount0Min: "0",
            amount1Min: "0",
            deadline: Math.floor(Date.now() / 1000)
        })

        // Calculate minimum amounts with slippage protection
        const amount0Min = new BigNumber(amount0Expected)
            .multipliedBy(100 - slippage)
            .dividedBy(100)
            .integerValue()
            .toString(10)
        const amount1Min = new BigNumber(amount1Expected)
            .multipliedBy(100 - slippage)
            .dividedBy(100)
            .integerValue()
            .toString(10)

        // Remove liquidity
        const deadline = Math.floor(Date.now() / 1000) + 60 * 20 // 20 minutes

        // First decrease liquidity
        // Create wallet and sign transactions
        const wallet = new this.chain.connector.Wallet(this.wallet.private, this.chain.https)
        const positionManagerWithSigner = positionManager.contract.connect(wallet)

        // First decrease liquidity
        const decreaseTx = await positionManagerWithSigner.decreaseLiquidity({
            tokenId: _position,
            liquidity: liquidityToRemove,
            amount0Min,
            amount1Min,
            deadline
        })

        // Then collect all fees and tokens (use max uint128)
        const collectTx = await positionManagerWithSigner.collect({
            tokenId: _position,
            recipient: this.wallet.address,
            amount0Max: MAX_UINT128.toString(), // Protocol requires amount0/amount1 naming
            amount1Max: MAX_UINT128.toString() // Protocol requires amount0/amount1 naming
        })

        // Convert amounts to decimal values
        const amount0 = new BigNumber(decreaseTx.events.Collect.returnValues.amount0).dividedBy(new BigNumber(10).pow(token0.configs.decimals)).toString()
        const amount1 = new BigNumber(decreaseTx.events.Collect.returnValues.amount1).dividedBy(new BigNumber(10).pow(token1.configs.decimals)).toString()
        const fees0 = new BigNumber(collectTx.events.Collect.returnValues.amount0).dividedBy(new BigNumber(10).pow(token0.configs.decimals)).toString()
        const fees1 = new BigNumber(collectTx.events.Collect.returnValues.amount1).dividedBy(new BigNumber(10).pow(token1.configs.decimals)).toString()

        return {
            token0: {
                amount: amount0,
                fees: fees0
            },
            token1: {
                amount: amount1,
                fees: fees1
            },
            decreaseTx,
            collectTx
        }
    } catch (error) {
        return { error: error.message }
    }
}

export default remove
