import { BigNumber } from "../../Utils.js"
import { loadContract, loadABI } from "../../Utils/contracts.js"

export const getAmountsOut = async function ({ token0: _token0, token1: _token1, amount } = {}) {
    try {
        const router = { configs: await loadContract({ chain: this.configs.chain, address: this.configs.router }) }
        if (!router.configs) throw new Error("Router contract not found")
        router.ABI = await loadABI({ ABI: router.configs.ABI, methods: ["getAmountsOut"] })
        if (!router.ABI) throw new Error("Router ABI not found")
        router.contract = this.chain.Contract({ address: this.configs.router, ABI: router.ABI })

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

        const amountBN = new BigNumber(amount).multipliedBy(new BigNumber(10).pow(token0.configs.decimals)).toString(10)
        const amounts = await router.contract.getAmountsOut(amountBN, [_token0, _token1])

        return {
            token0: { quantity: amount },
            token1: { quantity: new BigNumber(amounts[1]).dividedBy(new BigNumber(10).pow(token1.configs.decimals)).toNumber() }
        }
    } catch (error) {
        throw error
    }
}

export default getAmountsOut
