import { BigNumber, loadContract, loadABI } from "../../Utils.js"

export const swap = async function ({ token0: _token0, token1: _token1, amount0, slippage = 0.5 } = {}) {
    if (!this.wallet) throw new Error("Wallet not connected")
    try {
        // Get router contract
        const router = { configs: await loadContract({ chain: this.configs.chain, address: this.configs.router }) }
        if (!router.configs) throw new Error("Router contract not found")
        router.ABI = await loadABI({ ABI: router.configs.ABI, methods: ["swapExactTokensForTokens", "getAmountsOut"] })
        if (!router.ABI) throw new Error("Router ABI not found")
        router.contract = this.chain.Contract({ address: this.configs.router, ABI: router.ABI })

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
        token1.ABI = await loadABI({ ABI: token1.configs.ABI, methods: ["decimals"] })
        if (!token1.ABI) throw new Error("Token1 ABI not found")
        token1.contract = this.chain.Contract({ address: _token1, ABI: token1.ABI })
        if (!token1.configs?.decimals) token1.configs.decimals = await token1.contract.decimals()

        // Convert amount to big number
        token0.amountBN = new BigNumber(amount0).multipliedBy(new BigNumber(10).pow(token0.configs?.decimals)).toString(10)

        // Get expected output amount
        const amounts = await router.contract.getAmountsOut(token0.amountBN, [_token0, _token1])
        token1.amountMin = new BigNumber(amounts[1])
            .multipliedBy(new BigNumber(100 - slippage))
            .dividedBy(100)
            .integerValue()
            .toString(10)

        // Approve router to spend tokens
        // Create wallet and sign transactions
        const wallet = new this.chain.connector.Wallet(this.wallet.private, this.chain.https)
        const token0WithSigner = token0.contract.connect(wallet)
        await token0WithSigner.approve(this.configs.router, token0.amountBN)

        // Execute swap
        const deadline = Math.floor(Date.now() / 1000) + 60 * 20 // 20 minutes
        const routerWithSigner = router.contract.connect(wallet)
        const TX = await routerWithSigner.swapExactTokensForTokens(token0.amountBN, token1.amountMin, [_token0, _token1], this.wallet.address, deadline)

        return {
            token0: { amount: new BigNumber(token0.amountBN).dividedBy(new BigNumber(10).pow(token0.configs?.decimals)).toNumber() },
            token1: { amount: new BigNumber(amounts[1]).dividedBy(new BigNumber(10).pow(token1.configs?.decimals)).toNumber() },
            TX
        }
    } catch (error) {
        return { error: error.message }
    }
}

export default swap
