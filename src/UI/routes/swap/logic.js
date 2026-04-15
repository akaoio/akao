import { fiatValue } from "/core/Utils/contracts.js"

export class Logic {
    static options(chain, pools, Chains) {
        const opts = []
        if (Chains[chain]?.currencies) for (const [address, contract] of Object.entries(Chains[chain].currencies)) opts.push({ address, configs: contract })

        if (pools) {
            const known = new Set(opts.map((o) => o.address))
            for (const pool of Object.values(pools))
                for (const token of [pool.token0, pool.token1])
                    if (token?.address && !known.has(token.address) && token.configs) {
                        opts.push({ address: token.address, configs: token.configs })
                        known.add(token.address)
                    }
        }
        return opts
    }

    static balance(chain, from, balances) {
        if (!chain || !from) return null
        const balance = balances?.[chain]?.[from.address]
        if (balance === undefined) return null
        return Number(balance)
    }

    static find(from, to, chain, pools, Dexs) {
        if (!chain || !from || !to || !pools) return null
        for (const [address, pool] of Object.entries(pools)) {
            const match = (pool.token0?.address === from && pool.token1?.address === to) || (pool.token0?.address === to && pool.token1?.address === from)
            if (!match) continue
            const dex = Dexs[`${chain}.${pool.dex}${pool.version}`]
            if (!dex) continue
            return { dex, pool, address }
        }
        return null
    }

    static check(amount, chain, address, balances) {
        const balance = balances?.[chain]?.[address]
        if (balance === undefined) return true
        return amount <= Number(balance)
    }

    static async quote({ from, to, amount, chain, pools, Dexs, balances, fiat, forex, Wallets, address }) {
        if (!from || !to || !amount || !chain) return { error: null, amountOut: 0, fiatOut: 0, gasAmount: null, gasSymbol: null }

        if (!Logic.check(amount, chain, from.address, balances)) return { error: "insufficientBalance", amountOut: 0, fiatOut: 0, gasAmount: null, gasSymbol: null }

        const found = Logic.find(from.address, to.address, chain, pools, Dexs)
        if (!found) return { error: "nopoolFound", amountOut: 0, fiatOut: 0, gasAmount: null, gasSymbol: null }

        const { dex, pool } = found
        let result
        try {
            if (pool.version === "V2") result = await dex.quote({ token0: from.address, token1: to.address, amount })
            else if (pool.version === "V3") result = await dex.quote({ token0: from.address, token1: to.address, fee: pool.fee, amountIn: amount })
        } catch (e) {
            return { error: e?.message || String(e), amountOut: 0, fiatOut: 0, gasAmount: null, gasSymbol: null }
        }
        if (!result) return { error: null, amountOut: 0, fiatOut: 0, gasAmount: null, gasSymbol: null }

        const amountOut = result.token1?.quantity ?? result.amount ?? 0
        const fiatOut = await fiatValue({ chain, currency: to.configs, amount: amountOut, fiat, forex })

        let gasAmount = null
        let gasSymbol = null
        try {
            const wallet = Wallets?.[chain]
            if (wallet && address) {
                const fee = await wallet.fee({ to: pool.address, amount, currency: from.configs })
                if (fee) {
                    gasAmount = fee.amount
                    gasSymbol = fee.symbol
                }
            }
        } catch (_) {
            /* non-fatal */
        }

        return { error: null, amountOut, fiatOut, gasAmount, gasSymbol }
    }

    static async swap({ from, to, amount, slippage, chain, pools, Dexs, balances }) {
        if (!from || !to || !amount || !chain) return { success: false, error: "missingRequiredFields" }
        if (!Logic.check(amount, chain, from.address, balances)) return { success: false, error: "insufficientBalance" }

        const found = Logic.find(from.address, to.address, chain, pools, Dexs)
        if (!found) return { success: false, error: "nopoolFound" }
        const { dex, pool } = found
        try {
            const tx = await dex.swap({ token0: from.address, token1: to.address, amount0: amount, slippage, fee: pool?.fee })
            if (tx && !tx.error) return { success: true, error: null }
            return { success: false, error: "transactionError" }
        } catch (e) {
            return { success: false, error: e?.message || "transactionError" }
        }
    }
}

export default Logic
