import { DEV } from "./Utils/environment.js"
import { sha256 } from "./Utils/crypto.js"
import { HDNodeWallet, getBytes } from "./Ethers.js"
import zen, { initZEN } from "./ZEN.js"

export const DEV_PLATFORM_SEED = "seed"

export async function devplatform({ seed = DEV_PLATFORM_SEED, runtime = null } = {}) {
    const z = runtime?.pair ? runtime : (await initZEN(), zen)
    const pair = await z.pair(null, { seed })
    const root = HDNodeWallet.fromSeed(getBytes("0x" + sha256(seed)))

    return {
        seed,
        pair,
        pub: pair.pub,
        epub: pair.epub,
        xpub: root.neuter().extendedKey
    }
}

export async function siteplatform(site, { dev = DEV, seed = DEV_PLATFORM_SEED, runtime = null } = {}) {
    if (!site || typeof site !== "object") throw new Error("siteRequired")

    const platform = { ...(site.platform || {}) }
    if (platform.pub && platform.epub && platform.xpub) return platform
    if (!dev) return platform

    const fallback = await devplatform({ seed, runtime })
    return {
        ...fallback,
        ...platform,
        pair: platform.pair || fallback.pair,
        pub: platform.pub || fallback.pub,
        epub: platform.epub || fallback.epub,
        xpub: platform.xpub || fallback.xpub
    }
}

export async function patchsiteplatform(site, options = {}) {
    const platform = await siteplatform(site, options)
    if (!site || typeof site !== "object") throw new Error("siteRequired")
    site.platform = platform
    return platform
}
