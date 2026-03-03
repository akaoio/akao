import { load, write } from "../src/core/Utils.js"
import { ethers } from "ethers"
import { icons, color } from "../src/core/Colors.js"

const failedRPCs = new Map()
const abiCache = new Map()
const RETRY_DELAY = 5 * 60 * 1000

const normalize = (address) => {
    try {
        return ethers.getAddress(address)
    } catch {
        return null
    }
}

const makeProvider = ({ rpc, chainName, chainId }) => {
    try {
        const network = {
            chainId: Number(chainId),
            name: (chainName || `chain-${chainId}`).toLowerCase()
        }
        if (rpc.startsWith("ws")) return new ethers.WebSocketProvider(rpc, network)
        const options = { timeout: 10000, staticNetwork: network }
        return new ethers.JsonRpcProvider(rpc, network, options)
    } catch {
        return null
    }
}

const getWorkingRPC = async (chainName, chainConfigs) => {
    const now = Date.now()
    const allRPCs = Array.isArray(chainConfigs?.RPCs) ? chainConfigs.RPCs : []
    const rpcs = [...allRPCs.filter((rpc) => rpc?.startsWith?.("http")), ...allRPCs.filter((rpc) => rpc?.startsWith?.("ws"))]

    for (const rpc of rpcs) {
        const failedAt = failedRPCs.get(rpc)
        if (failedAt && now - failedAt < RETRY_DELAY) continue
        if (failedAt && now - failedAt >= RETRY_DELAY) failedRPCs.delete(rpc)

        const provider = makeProvider({ rpc, chainName, chainId: chainConfigs.id })
        if (!provider) continue

        try {
            await provider.getBlockNumber()
            console.log(color.info(`${color.primary(chainName)} [${color.info(chainConfigs.id)}] Using RPC: ${color.secondary(rpc)}`))
            return { provider, rpc }
        } catch {
            failedRPCs.set(rpc, now)
            if (provider?.destroy) await provider.destroy()
        }
    }

    return {}
}

const loadABI = async (name) => {
    if (!name) return
    if (abiCache.has(name)) return abiCache.get(name)
    const abi = await load(["src", "statics", "ABIs", `${name}.json`])
    if (!Array.isArray(abi) || !abi.length) return
    abiCache.set(name, abi)
    return abi
}

const getDecimals = async (provider, currency) => {
    if (!currency?.address || !currency?.ABI) return null
    const abi = await loadABI(currency.ABI)
    if (!abi) return null

    try {
        const contract = new ethers.Contract(currency.address, abi, provider)
        const decimals = await contract.decimals()
        return Number(decimals)
    } catch (error) {
        console.error(color.error(`Failed to fetch decimals for ${currency.address}: ${error.message}`))
        return null
    }
}

const scanChainCurrencies = async (chainName, chainData, { force = false } = {}) => {
    const configs = chainData?.configs
    const currencies = chainData?.currencies
    if (!configs?.id || !Array.isArray(currencies)) return { updated: false, scanned: 0 }

    console.log(color.header(`\n${icons.chain} ${color.primary(chainName)} [${color.info(configs.id)}]`))

    let { provider } = await getWorkingRPC(chainName, configs)
    if (!provider) {
        console.error(color.error(`No available RPC for ${chainName}`))
        return { updated: false, scanned: 0 }
    }

    let updated = false
    let scanned = 0
    const chainId = Number(configs.id)

    try {
        for (const currency of currencies) {
            if (currency?.chain !== chainId) {
                currency.chain = chainId
                updated = true
            }

            if (currency?.address) {
                const normalized = normalize(currency.address)
                if (normalized && normalized !== currency.address) {
                    currency.address = normalized
                    updated = true
                }
            }

            if (currency?.wrapped) {
                const normalizedWrapped = normalize(currency.wrapped)
                if (normalizedWrapped && normalizedWrapped !== currency.wrapped) {
                    currency.wrapped = normalizedWrapped
                    updated = true
                }
            }

            if (!currency?.address || !currency?.ABI) continue
            scanned++

            if (currency.decimals !== undefined && !force) continue

            const decimals = await getDecimals(provider, currency)
            if (decimals === null || Number.isNaN(decimals)) continue

            if (currency.decimals !== decimals) {
                currency.decimals = decimals
                updated = true
            }

            console.log(color.ok(`${color.primary(currency.name || currency.address)} ${color.secondary(currency.address)} -> ${decimals}`))
        }

        for (const native of currencies.filter((currency) => !currency?.address && currency?.wrapped)) {
            const wrapped = currencies.find((currency) => currency?.address?.toLowerCase?.() === native.wrapped.toLowerCase())
            if (wrapped?.decimals !== undefined && native.decimals !== wrapped.decimals) {
                native.decimals = wrapped.decimals
                updated = true
            }
        }

        if (updated) {
            await write(["src", "statics", "chains", chainName, "currencies.json"], currencies)
            console.log(color.ok(`Updated currencies for ${chainName}`))
        }

        return { updated, scanned }
    } finally {
        if (provider?.destroy) await provider.destroy()
    }
}

export const scanTokenDecimals = async ({ chain = null, force = false } = {}) => {
    console.log(color.header(`\n${icons.start} Currency Scanner`))

    const chains = await load(["src", "statics", "chains"])
    if (!chains || typeof chains !== "object") return []

    const entries = Object.entries(chains).filter(([name]) => (!chain ? true : name.toLowerCase() === chain.toLowerCase()))
    if (!entries.length) {
        console.error(color.error(`No chain found for: ${chain}`))
        return []
    }

    const results = []
    for (const [chainName, chainData] of entries) {
        const result = await scanChainCurrencies(chainName, chainData, { force })
        results.push({ chain: chainName, ...result })
    }

    const scanned = results.reduce((sum, item) => sum + item.scanned, 0)
    const updated = results.filter((item) => item.updated).length

    console.log(color.header(`\n${icons.done} Currency Scan Summary`))
    console.log(color.ok(`Chains scanned: ${results.length}`))
    console.log(color.ok(`Token contracts checked: ${scanned}`))
    console.log(color.ok(`Chains updated: ${updated}`))

    return results
}
