import { Access, setWallet } from "/core/Access.js"
import { fiatValue } from "/core/Utils/contracts.js"
import zen from "/core/ZEN.js"
import { writeLabel, readLabel, subscribeLabel } from "./label.js"

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

    static async balance({ wallet, currency, fiat, forex, address }) {
        if (!wallet || !currency) return { raw: null, amount: 0 }
        // Use explicit address when provided (e.g. previewing a different wallet id)
        const raw = address
            ? await wallet.chain.balance({ address, currency })
            : await wallet.balance({ currency })
        if (typeof raw === "undefined") return { raw: null, amount: 0 }
        const chain = Number(wallet.chain?.id)
        const amount = await fiatValue({ chain, currency, amount: Number(raw) || 0, fiat, forex }) || 0
        return { raw, amount }
    }

    static async seed() {
        const rawSeed = Access.get("seed")
        if (!rawSeed) return null
        if (!zen) return null
        return zen.hash(rawSeed, "wallet")
    }

    static writeLabel(pair, id, label) { return writeLabel(pair, id, label) }
    static readLabel(pub, id) { return readLabel(pub, id) }
    static subscribeLabel(pub, id, cb) { return subscribeLabel(pub, id, cb) }

    static id() {
        return Number(Access.get("wallet")?.id || 0)
    }

    static total() {
        return Number(Access.get("wallet")?.total || 0)
    }

    static setid(value, step, current) {
        value = Number(value)
        const total = value >= current ? value + 1 : current
        setWallet({ id: value, total: total !== current ? total : undefined })
        return value
    }

    static settotal(value) {
        setWallet({ total: Number(value) })
        return Number(value)
    }
}

export default Logic
