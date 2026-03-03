import { NODE } from "./environment.js"
import { load, exist } from "../FS.js"
import { Indexes, Lives, Statics } from "../Stores.js"

export const loadContract = async ({ chain, address } = {}) => {
    if (!chain || !address) return
    // Initialize Statics chain structure if needed
    if (!Statics.chains) Statics.chains = {}
    if (!Statics.chains[chain]) Statics.chains[chain] = {}
    if (!Statics.chains[chain].contracts) Statics.chains[chain].contracts = {}

    // Try to get contract from different sources
    const indexes = await Indexes.Statics.get("contracts").get(chain).get(address).once()
    const statics = Statics.chains[chain].contracts?.[address]
    let contract = indexes || statics || (await load(["statics", "chains", chain, "contracts", `${address}.json`]))
    if (!contract) return

    // Cache the contract for future use
    if (!indexes) Indexes.Statics.get("contracts").get(chain).get(address).put(contract)
    if (!statics) Statics.chains[chain].contracts[address] = contract
    return contract
}

export const loadABI = async ({ ABI, methods = [] } = {}) => {
    if (!Statics.ABIs) Statics.ABIs = Statics.ABIs || {}
    if (!Statics.ABIs[ABI]) Statics.ABIs[ABI] = Statics.ABIs[ABI] || {}
    const result = []

    if (typeof methods == "string") methods = [methods]

    await Promise.all(
        methods.map(async (method) => {
            // Check IndexedDB first
            const indexes = await Indexes.Statics.get("ABIs").get(ABI).get(method).once()
            const statics = Statics.ABIs[ABI][method]

            let data = indexes || statics

            const staticsPath = ["statics", "ABIs", ABI, `${method}.json`]
            if (!data && (!NODE || (await exist(staticsPath)))) data = await load(staticsPath)
            if (!data) return
            if (!indexes) Indexes.Statics.get("ABIs").get(ABI).get(method).put(data)
            if (!statics) Statics.ABIs[ABI][method] = data
            result.push(data)
        })
    )

    return result
}

export const fiatValue = async ({ chain, currency, amount, fiat, forex, inverse = false }) => {
    // Quick returns for common cases
    if (amount === 0) return 0

    // Use default chain from currency if chain is not provided
    chain = chain || currency?.chain

    // Use global forex rates if not explicitly provided
    forex = forex || Lives?.forex

    // Exit if essential parameters are missing
    if (!chain || !currency || !fiat || !forex) return

    // Get fiat rates for the given chain and currency
    const rates = await fiatRates({ chain, currency })
    if (!rates) return

    // Check for direct conversion first with non-zero rate
    if (rates[fiat] && rates[fiat] > 0) {
        return inverse ? amount / rates[fiat] : amount * rates[fiat]
    }

    // Try forex conversion through available rates, ignoring zero rates
    for (const [baseFiat, rate] of Object.entries(rates)) {
        // Skip zero or negative rates
        if (rate <= 0) continue

        const forexRate = inverse ? forex[fiat]?.[baseFiat] : forex[baseFiat]?.[fiat]
        // Only process if forexRate is valid (non-zero, non-null)
        if (forexRate && forexRate > 0) {
            return inverse ? amount / rate / forexRate : amount * rate * forexRate
        }
    }

    // Return undefined if no valid conversion found
    return undefined
}

export const fiatRates = async ({ chain, currency } = {}) => {
    // Use default chain from currency if chain is not provided
    chain = chain || currency?.chain

    // Exit if Lives.pools or essential parameters are missing
    if (!Lives?.pools || !chain || !currency) return

    // For native currencies, switch to their wrapped token to fetch rates
    if (!currency.address && currency.wrapped) {
        const wrapped = await loadContract({ chain, address: currency.wrapped })
        if (wrapped) return await fiatRates({ chain, currency: wrapped })
    }

    // Quick return for stablecoins
    if (currency?.group === "stables" && currency?.fiat) {
        return { [currency.fiat]: 1 }
    }

    // Get all rates for this token from pools
    const pools = Lives?.pools?.[chain]
    if (!pools) return

    // Find all pools that contain this token and have valid rates
    const relevantPools = Object.values(pools).filter((pool) => {
        if (!pool.pairs) return false
        const isToken0 = pool.token0?.address === (currency?.address || currency)
        const isToken1 = pool.token1?.address === (currency?.address || currency)
        if (!isToken0 && !isToken1) return false

        // Check if pool has valid rates for this token
        return isToken0 ? pool.pairs[pool.token0.address]?.[pool.token1.address] > 0 : pool.pairs[pool.token1.address]?.[pool.token0.address] > 0
    })

    if (!relevantPools.length) return

    // Collect all rates where this token is paired with stablecoins
    const rates = {}
    relevantPools.forEach((pool) => {
        const pairs = pool.pairs
        if (!pairs) return

        // Check if this token is token0 or token1 in the pool
        const isToken0 = pool.token0?.address === (currency?.address || currency)
        const otherToken = isToken0 ? pool.token1 : pool.token0
        const rate = isToken0 ? pairs[pool.token0.address]?.[pool.token1.address] : pairs[pool.token1.address]?.[pool.token0.address]

        if (rate > 0) {
            rates[otherToken.address] = rate
        }
    })

    // Find stablecoins among the paired tokens
    const valids = Object.keys(rates)
    const stables = Statics.chains[chain]?.stables.filter((address) => valids.includes(address))

    if (!Array.isArray(stables) || !stables.length) return

    // Build rate map efficiently, only including positive rates
    const list = {}
    for (const address of stables) {
        const stable = await loadContract({ chain, address })
        if (!stable?.fiat) continue
        const rate = rates[address]

        // Skip zero or negative rates
        if (rate <= 0) continue

        if (!list[stable.fiat]) {
            list[stable.fiat] = [rate]
        } else if (!list[stable.fiat].includes(rate)) {
            list[stable.fiat].push(rate)
        }
    }

    // Calculate final rates using only positive values
    const finalRates = {}
    for (const [fiat, rateArray] of Object.entries(list)) {
        const validRates = rateArray.filter((rate) => rate > 0)
        if (validRates.length > 0) {
            finalRates[fiat] = validRates.length === 1 ? validRates[0] : validRates.reduce((sum, rate) => sum + rate, 0) / validRates.length
        }
    }

    return Object.keys(finalRates).length ? finalRates : undefined
}
