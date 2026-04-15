import { BigNumber } from "../../Utils.js"
import { loadContract, loadABI } from "../../Utils/contracts.js"

export const quote = async function ({ token0: _token0, token1: _token1, amount, amountIn, fee = 3000 } = {}) {
    const inputAmount = amountIn ?? amount
    if (inputAmount === undefined || inputAmount === null) throw new Error("Input amount not found")

    const quoter = { configs: await loadContract({ chain: this.configs.chain, address: this.configs.quoter }) }
    if (!quoter.configs) throw new Error("Quoter contract not found")
    quoter.ABI = await loadABI({ ABI: quoter.configs.ABI, methods: ["quoteExactInputSingle"] })
    if (!quoter.ABI) throw new Error("Quoter ABI not found")
    quoter.contract = this.chain.Contract({ address: this.configs.quoter, ABI: quoter.ABI })

    const token0 = { configs: await loadContract({ chain: this.configs.chain, address: _token0 }) }
    if (!token0.configs) throw new Error("Token0 contract not found")
    if (!token0.configs?.decimals) {
        token0.ABI = await loadABI({ ABI: token0.configs.ABI, methods: ["decimals"] })
        if (!token0.ABI) throw new Error("Token0 ABI not found")
        token0.contract = this.chain.Contract({ address: _token0, ABI: token0.ABI })
        token0.configs.decimals = await token0.contract.decimals()
    }

    const token1 = { configs: await loadContract({ chain: this.configs.chain, address: _token1 }) }
    if (!token1.configs) throw new Error("Token1 contract not found")
    if (!token1.configs?.decimals) {
        token1.ABI = await loadABI({ ABI: token1.configs.ABI, methods: ["decimals"] })
        if (!token1.ABI) throw new Error("Token1 ABI not found")
        token1.contract = this.chain.Contract({ address: _token1, ABI: token1.ABI })
        token1.configs.decimals = await token1.contract.decimals()
    }

    const amountInBN = new BigNumber(inputAmount).multipliedBy(new BigNumber(10).pow(token0.configs.decimals)).toFixed(0)
    const quotedAmount = await quoter.contract.quoteExactInputSingle(_token0, _token1, fee, amountInBN, "0")

    return {
        token0: {
            quantity: Number(inputAmount),
            quantityBN: amountInBN
        },
        token1: {
            quantity: new BigNumber(quotedAmount).dividedBy(new BigNumber(10).pow(token1.configs.decimals)).toNumber(),
            quantityBN: quotedAmount.toString()
        }
    }
}

export default quote