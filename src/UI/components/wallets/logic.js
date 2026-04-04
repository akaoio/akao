import { fiatValue } from "/core/Utils/contracts.js"

export class Logic {
    static currencies(Chains) {
        const map = {}
        for (const chain of Object.values(Chains))
            for (const currency of Object.values(chain.currencies)) {
                if (!currency?.name) continue
                map[currency.name] = { name: currency.name, symbol: currency.symbol }
            }
        return Object.values(map).sort((a, b) => a.name.localeCompare(b.name))
    }

    static chains(Chains, currency = null) {
        return Object.values(Chains)
            .filter(chain => !currency || Object.values(chain.currencies).some(c => c.name === currency))
            .map(chain => ({
                id: String(chain.id),
                name: chain.configs.name || String(chain.id),
                symbol: chain.configs.symbol
            }))
    }

    static currency(wallet, name) {
        if (!wallet?.chain?.currencies || !name) return null
        return Object.values(wallet.chain.currencies).find(c => c.name === name) || null
    }

    static async balance({ wallet, currency, fiat, forex }) {
        if (!wallet || !currency) return { raw: null, amount: 0 }
        const raw = await wallet.balance({ currency })
        if (typeof raw === "undefined") return { raw: null, amount: 0 }
        const chain = Number(wallet.chain?.id)
        const amount = await fiatValue({ chain, currency, amount: Number(raw) || 0, fiat, forex }) || 0
        return { raw, amount }
    }
}

export default Logic
