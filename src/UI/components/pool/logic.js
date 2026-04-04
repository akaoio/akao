import DB from "/core/DB.js"
import { fiatValue } from "/core/Utils/contracts.js"

export class Logic {
    static format(rate) {
        if (!rate) return "0"
        return rate < 0.0001 ? rate.toExponential(4) : rate < 1 ? rate.toFixed(6) : rate.toFixed(2)
    }

    static async prices({ pool, fiat, forex }) {
        if (!pool?.token0?.configs || !pool?.token1?.configs) {
            return { price0: 0, price1: 0, tvl: 0, rate01: 0, rate10: 0 }
        }
        const chain = pool.chain
        const t0 = pool.token0
        const t1 = pool.token1
        const price0 = await fiatValue({ chain, currency: t0.configs, amount: t0.quantity || 0, fiat, forex }) || 0
        const price1 = await fiatValue({ chain, currency: t1.configs, amount: t1.quantity || 0, fiat, forex }) || 0
        return {
            price0,
            price1,
            tvl: price0 + price1,
            rate01: pool.pairs?.[t0.address]?.[t1.address] || 0,
            rate10: pool.pairs?.[t1.address]?.[t0.address] || 0,
        }
    }

    static async dex(id) {
        const dexs = await DB.get(["statics", "dexs.json"])
        return dexs?.[id] || null
    }

}

export default Logic
