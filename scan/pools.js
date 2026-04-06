import { FS } from "../src/core/FS.js"
import { ethers } from "ethers"
import { icons, color } from "../src/core/Colors.js"

const failedRPCs = new Map()
const abiCache = new Map()
const RETRY_DELAY = 2 * 60 * 1000
const RPC_SWITCH_DELAY = 2000
const CALLS_PER_MINUTE = 30
const V2_FEE = 3000
const V3_FEES = [100, 500, 3000, 10000]

const rpcCalls = new Map()

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

const checkRateLimit = (rpc) => {
    const now = Date.now()
    const minute = Math.floor(now / 60000)
    const calls = rpcCalls.get(rpc) || new Map()
    const count = calls.get(minute) || 0

    if (count >= CALLS_PER_MINUTE) return false

    calls.set(minute, count + 1)
    rpcCalls.set(rpc, calls)
    return true
}

const getWorkingRPC = async (chainName, chainConfigs) => {
    const now = Date.now()
    const allRPCs = Array.isArray(chainConfigs?.RPCs) ? chainConfigs.RPCs : []
    const rpcs = [...allRPCs.filter((rpc) => rpc?.startsWith?.("http")), ...allRPCs.filter((rpc) => rpc?.startsWith?.("ws"))]

    for (const rpc of rpcs) {
        const failedAt = failedRPCs.get(rpc)
        if (failedAt && now - failedAt < RETRY_DELAY) continue
        if (failedAt && now - failedAt >= RETRY_DELAY) failedRPCs.delete(rpc)
        if (!checkRateLimit(rpc)) continue

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

    const abi = await FS.load(["src", "statics", "ABIs", `${name}.yaml`])
    if (!Array.isArray(abi) || !abi.length) return

    abiCache.set(name, abi)
    return abi
}

const verifyPool = async (provider, poolABIName, poolAddress, version) => {
    try {
        const code = await provider.getCode(poolAddress)
        if (code === "0x" || code === "0x0") return false
    } catch {
        return false
    }

    try {
        const abi = await loadABI(poolABIName)
        if (!abi) return false

        const contract = new ethers.Contract(poolAddress, abi, provider)

        if (version === "V2") {
            const [token0, token1, reserves] = await Promise.all([contract.token0(), contract.token1(), contract.getReserves()])
            return Boolean(token0 && token1 && reserves && reserves[0] && reserves[1])
        }

        const [token0, token1, fee] = await Promise.all([contract.token0(), contract.token1(), contract.fee()])
        return Boolean(token0 && token1 && fee !== undefined)
    } catch {
        return false
    }
}

const pool = async (provider, factory, token0, token1, version, fee = null) => {
    try {
        const code = await provider.getCode(factory.address)
        if (code === "0x" || code === "0x0") return null

        const abi = await loadABI(factory.ABI)
        if (!abi) return null

        const contract = new ethers.Contract(factory.address, abi, provider)
        const method = version === "V3" ? "getPool" : "getPair"

        const args = version === "V3" ? [token0, token1, fee] : [token0, token1]
        const address = await contract[method](...args)
        if (!address || address === "0x0000000000000000000000000000000000000000") return null

        const normalizedAddress = normalize(address)
        if (!normalizedAddress) return null

        return {
            address: normalizedAddress,
            token0,
            token1,
            ...(fee ? { fee } : {})
        }
    } catch {
        return null
    }
}

const orderTokens = (tokenA, tokenB) => {
    const a = normalize(tokenA)
    const b = normalize(tokenB)
    if (!a || !b) return null

    return a.toLowerCase() < b.toLowerCase() ? { token0: a, token1: b } : { token0: b, token1: a }
}

const savePools = async ({ chainName, dex, version, pools }) => {
    await FS.write(["src", "statics", "chains", chainName, "defis", dex, version, "pools.yaml"], pools)
}

const scanVersion = async ({ chainName, chainConfigs, chainData, dex, version, versionData, provider, rpc, existingAddresses }) => {
    const contracts = Array.isArray(versionData?.configs?.contracts) ? versionData.configs.contracts : []
    const factory = contracts.find((contract) => contract?.type === "factory")
    if (!factory) return { provider, rpc, added: 0 }

    const currencies = Array.isArray(chainData?.currencies) ? chainData.currencies : []
    const pairs = []
    currencies.forEach((currencyA, index) => {
        currencies.slice(index + 1).forEach((currencyB) => {
            if (currencyA?.address && currencyB?.address) pairs.push({ tokenA: currencyA, tokenB: currencyB })
        })
    })

    const currentPools = Array.isArray(versionData?.pools) ? versionData.pools : []
    let added = 0

    const batchSize = 2
    for (let i = 0; i < pairs.length; i += batchSize) {
        const batch = pairs.slice(i, i + batchSize)

        const results = await Promise.all(
            batch.map(async ({ tokenA, tokenB }) => {
                const ordered = orderTokens(tokenA.address, tokenB.address)
                if (!ordered) return null

                const fees = version === "V2" ? [V2_FEE] : V3_FEES
                for (const fee of fees) {
                    if (!checkRateLimit(rpc)) {
                        if (provider?.destroy) await provider.destroy()
                        const next = await getWorkingRPC(chainName, chainConfigs)
                        if (!next.provider) return null
                        provider = next.provider
                        rpc = next.rpc
                    }

                    const $pool = await pool(provider, factory, ordered.token0, ordered.token1, version, fee)
                    if (!$pool || existingAddresses.has($pool.address.toLowerCase())) continue

                    const poolABI = `${dex}${version}Pool`
                    const valid = await verifyPool(provider, poolABI, $pool.address, version)
                    if (!valid) {
                        failedRPCs.set(rpc, Date.now())
                        await new Promise((resolve) => setTimeout(resolve, RPC_SWITCH_DELAY))
                        continue
                    }

                    return {
                        chain: Number(chainConfigs.id),
                        type: "pool",
                        dex,
                        version,
                        ABI: poolABI,
                        ...$pool
                    }
                }

                return null
            })
        )

        for (const pool of results.filter(Boolean)) {
            if (existingAddresses.has(pool.address.toLowerCase())) continue
            currentPools.push(pool)
            existingAddresses.add(pool.address.toLowerCase())
            added++

            console.log(
                color.ok(
                    `${color.primary(chainName)} [${color.info(chainConfigs.id)}] ${color.primary(pool.dex)} ${color.secondary(pool.version)} ${color.secondary(pool.address)}`
                )
            )
        }

        await savePools({ chainName, dex, version, pools: currentPools })
        versionData.pools = currentPools
        await new Promise((resolve) => setTimeout(resolve, 1000))
    }

    return { provider, rpc, added }
}

const scanChainPools = async (chainName, chainData) => {
    const chainConfigs = chainData?.configs
    if (!chainConfigs?.id || !chainData?.defis) return 0

    console.log(color.header(`\n${icons.chain} ${color.primary(chainName)} [${color.info(chainConfigs.id)}]`))

    let { provider, rpc } = await getWorkingRPC(chainName, chainConfigs)
    if (!provider) {
        console.error(color.error(`No available RPC for ${chainName}`))
        return 0
    }

    let totalAdded = 0

    try {
        for (const [dex, dexData] of Object.entries(chainData.defis)) {
            for (const [version, versionData] of Object.entries(dexData || {})) {
                const existingPools = Array.isArray(versionData?.pools) ? versionData.pools : []
                const existingAddresses = new Set(existingPools.map((pool) => pool.address.toLowerCase()))

                const result = await scanVersion({
                    chainName,
                    chainConfigs,
                    chainData,
                    dex,
                    version,
                    versionData,
                    provider,
                    rpc,
                    existingAddresses
                })

                provider = result.provider
                rpc = result.rpc
                totalAdded += result.added
            }
        }

        return totalAdded
    } finally {
        if (provider?.destroy) await provider.destroy()
    }
}

export const scanPools = async ({ chain = null } = {}) => {
    console.log(color.header(`\n${icons.start} Pool Scanner`))

    const chains = await FS.load(["src", "statics", "chains"])
    if (!chains || typeof chains !== "object") return []

    const entries = Object.entries(chains).filter(([name]) => (!chain ? true : name.toLowerCase() === chain.toLowerCase()))
    if (!entries.length) {
        console.error(color.error(`No chain found for: ${chain}`))
        return []
    }

    const results = []
    for (const [chainName, chainData] of entries) {
        const added = await scanChainPools(chainName, chainData)
        results.push({ chain: chainName, added })
    }

    const total = results.reduce((sum, item) => sum + item.added, 0)
    console.log(color.header(`\n${icons.done} Pool Scan Summary`))
    console.log(color.ok(`Chains scanned: ${results.length}`))
    console.log(color.ok(`New pools: ${total}`))

    return results
}
