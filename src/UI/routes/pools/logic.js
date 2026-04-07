export class Logic {

    static flatten(cached) {
        const pools = {}
        if (!cached || typeof cached !== "object") return pools
        for (const dexpools of Object.values(cached)) {
            if (!dexpools || typeof dexpools !== "object") continue
            for (const data of Object.values(dexpools)) {
                if (!data?.chain) continue
                if (!pools[data.chain]) pools[data.chain] = {}
                pools[data.chain][data.address] = data
            }
        }
        return pools
    }

    static filter(pools, params) {
        if (!pools) return []
        const filterChain = params?.chain ? Number(params.chain) : null
        const filterDex = params?.dex ? params.dex.toLowerCase() : null
        const rows = []
        for (const [chain, chainPools] of Object.entries(pools)) {
            if (filterChain && Number(chain) !== filterChain) continue
            for (const [address, pool] of Object.entries(chainPools)) {
                if (!pool?.token0 || !pool?.token1) continue
                if (filterDex && pool.dex?.toLowerCase() !== filterDex) continue
                rows.push({ chain: Number(chain), address })
            }
        }
        return rows
    }

    static extractFilters(pools) {
        const chains = new Set()
        const dexes = new Set()
        if (!pools || typeof pools !== "object") return { chains: [], dexes: [] }
        for (const [chain, chainPools] of Object.entries(pools)) {
            if (!chainPools || typeof chainPools !== "object") continue
            chains.add(Number(chain))
            for (const pool of Object.values(chainPools))
                if (pool?.dex) dexes.add(pool.dex.toLowerCase())
        }
        return {
            chains: [...chains].sort((a, b) => a - b),
            dexes: [...dexes].sort(),
        }
    }

}

export default Logic
