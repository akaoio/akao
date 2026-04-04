import { BigNumber } from "../../Utils.js"
import { loadContract, loadABI } from "../../Utils/contracts.js"

export const remove = async function ({ position: _position, token0: _token0, token1: _token1, amount, slippage = 0.5 } = {}) {
    if (!this.wallet) throw new Error("Wallet not connected")
    try {
        // Get router contract
        const router = { configs: await loadContract({ chain: this.configs.chain, address: this.configs.router }) }
        if (!router.configs) throw new Error("Router contract not found")
        router.ABI = await loadABI({ ABI: router.configs.ABI, methods: ["removeLiquidity"] })
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
        token1.ABI = await loadABI({ ABI: token1.configs.ABI, methods: ["decimals", "approve"] })
        if (!token1.ABI) throw new Error("Token1 ABI not found")
        token1.contract = this.chain.Contract({ address: _token1, ABI: token1.ABI })
        if (!token1.configs?.decimals) token1.configs.decimals = await token1.contract.decimals()

        // Get position token contract
        // _position might not exist. This code needs revision.
        const position = { configs: await loadContract({ chain: this.configs.chain, address: _position }) }
        if (!position.configs) throw new Error("position contract not found")
        position.ABI = await loadABI({ ABI: position.configs.ABI, methods: ["decimals", "approve", "totalSupply"] })
        if (!position.ABI) throw new Error("position ABI not found")
        position.contract = this.chain.Contract({ address: _position, ABI: position.ABI })
        if (!position.configs?.decimals) position.configs.decimals = await position.contract.decimals()

        // Convert position amount to big number
        position.amountBN = new BigNumber(amount).multipliedBy(new BigNumber(10).pow(position.configs?.decimals)).toString(10)

        // Calculate expected output amounts based on pool share
        const totalSupply = await position.contract.totalSupply()
        token0.reserve = await token0.contract.balanceOf(_position)
        token1.reserve = await token1.contract.balanceOf(_position)

        token0.expected = new BigNumber(token0.reserve).multipliedBy(position.amountBN).dividedBy(totalSupply).integerValue().toString(10)
        token1.expected = new BigNumber(token1.reserve).multipliedBy(position.amountBN).dividedBy(totalSupply).integerValue().toString(10)

        // Calculate minimum amounts with slippage
        token0.amountMin = new BigNumber(token0.expected)
            .multipliedBy(new BigNumber(100 - slippage))
            .dividedBy(100)
            .integerValue()
            .toString(10)
        token1.amountMin = new BigNumber(token1.expected)
            .multipliedBy(new BigNumber(100 - slippage))
            .dividedBy(100)
            .integerValue()
            .toString(10)

        // Approve router to spend position tokens
        // Create wallet and sign transactions
        const wallet = new this.chain.connector.Wallet(this.wallet.private, this.chain.https)
        const positionWithSigner = position.contract.connect(wallet)
        await positionWithSigner.approve(this.configs.router, position.amountBN)

        // Remove liquidity
        const deadline = Math.floor(Date.now() / 1000) + 60 * 20 // 20 minutes
        const routerWithSigner = router.contract.connect(wallet)
        const TX = await routerWithSigner.removeLiquidity(_token0, _token1, position.amountBN, token0.amountMin, token1.amountMin, this.wallet.address, deadline)

        return {
            token0: {
                amount: new BigNumber(token0.expected).dividedBy(new BigNumber(10).pow(token0.configs?.decimals)).toNumber()
            },
            token1: {
                amount: new BigNumber(token1.expected).dividedBy(new BigNumber(10).pow(token1.configs?.decimals)).toNumber()
            },
            TX
        }
    } catch (error) {
        return { error: error.message }
    }
}

export default remove