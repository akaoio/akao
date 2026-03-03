import { BigNumber, loadContract, loadABI } from "../../Utils.js"

// Helper to convert to token decimal amount
const toTokenDecimals = (amount, decimals) => {
    return new BigNumber(amount).multipliedBy(new BigNumber(10).pow(decimals)).toFixed(0)
}

// Helper to convert from token decimal amount
const fromTokenDecimals = (amount, decimals) => {
    return new BigNumber(amount).dividedBy(new BigNumber(10).pow(decimals)).toString()
}

export const addLiquidity = async function ({ token0: _token0, token1: _token1, amount0, amount1, fee = 3000, tickLower, tickUpper } = {}) {
    if (!this.wallet) throw new Error("Wallet not connected")
    try {
        // Get position manager contract
        const positionManager = { configs: await loadContract({ chain: this.configs.chain, address: this.configs.positionManager }) }
        if (!positionManager.configs) throw new Error("Position manager contract not found")
        positionManager.ABI = await loadABI({ ABI: positionManager.configs.ABI, methods: ["mint", "positions"] })
        if (!positionManager.ABI) throw new Error("Position manager ABI not found")
        positionManager.contract = this.chain.Contract({ address: this.configs.positionManager, ABI: positionManager.ABI })

        // Get token0 contract
        const token0 = { configs: await loadContract({ chain: this.configs.chain, address: _token0 }) }
        if (!token0.configs) throw new Error("Token0 contract not found")
        token0.ABI = await loadABI({ ABI: token0.configs.ABI, methods: ["decimals", "approve"] })
        if (!token0.ABI) throw new Error("Token0 ABI not found")
        token0.contract = this.chain.Contract({ address: _token0, ABI: token0.ABI })
        if (!token0.configs?.decimals) token0.configs.decimals = await token0.contract.decimals()

        // Get token1 contract
        const token1 = { configs: await loadContract({ chain: this.configs.chain, address: _token1 }) }
        if (!token1.configs) throw new Error("Token1 contract not found")
        token1.ABI = await loadABI({ ABI: token1.configs.ABI, methods: ["decimals", "approve"] })
        if (!token1.ABI) throw new Error("Token1 ABI not found")
        token1.contract = this.chain.Contract({ address: _token1, ABI: token1.ABI })
        if (!token1.configs?.decimals) token1.configs.decimals = await token1.contract.decimals()

        // Convert amounts to token decimals
        token0.amountBN = toTokenDecimals(amount0, token0.configs.decimals)
        token1.amountBN = toTokenDecimals(amount1, token1.configs.decimals)

        // Approve position manager to spend both tokens
        // Create wallet and sign transactions
        const wallet = new this.chain.connector.Wallet(this.wallet.private, this.chain.https)
        const token0WithSigner = token0.contract.connect(wallet)
        const token1WithSigner = token1.contract.connect(wallet)
        await token0WithSigner.approve(this.configs.positionManager, token0.amountBN)
        await token1WithSigner.approve(this.configs.positionManager, token1.amountBN)

        // Add liquidity
        const deadline = Math.floor(Date.now() / 1000) + 60 * 20 // 20 minutes
        const positionManagerWithSigner = positionManager.contract.connect(wallet)
        const TX = await positionManagerWithSigner.mint({
            token0: _token0, // Protocol requires token0/token1 naming
            token1: _token1, // Protocol requires token0/token1 naming
            fee,
            tickLower,
            tickUpper,
            amount0Desired: token0.amountBN, // Protocol requires amount0/amount1 naming
            amount1Desired: token1.amountBN, // Protocol requires amount0/amount1 naming
            amount0Min: "0", // Protocol requires amount0/amount1 naming
            amount1Min: "0", // Protocol requires amount0/amount1 naming
            recipient: this.wallet.address,
            deadline
        })

        // Get position token ID from event
        const position = TX.events.Transfer.returnValues.tokenId

        return {
            position,
            token0: {
                amount: fromTokenDecimals(token0.amountBN, token0.configs.decimals)
            },
            token1: {
                amount: fromTokenDecimals(token1.amountBN, token1.configs.decimals)
            },
            TX
        }
    } catch (error) {
        return { error: error.message }
    }
}

export default addLiquidity
