import { BROWSER, DEV } from "./Utils/environment.js"
import zen, { initZEN } from "./ZEN.js"

export const DEV_PLATFORM_SEED = "seed"

export function canDeriveDevPlatform({ browser = BROWSER, scope = globalThis } = {}) {
    return !browser || !!scope?.crypto?.subtle
}

export async function devplatform({ seed = DEV_PLATFORM_SEED, runtime = null } = {}) {
    const z = runtime?.pair ? runtime : (await initZEN(), zen)
    const pair = await z.pair(null, { seed })

    return {
        seed,
        pair,
        pub: pair.pub
    }
}

export async function siteplatform(site, { dev = DEV, seed = DEV_PLATFORM_SEED, runtime = null, browser = BROWSER, scope = globalThis } = {}) {
    if (!site || typeof site !== "object") throw new Error("siteRequired")

    const platform = { ...(site.platform || {}) }
    if (platform.pub) return platform
    if (!dev) return platform
    if (!canDeriveDevPlatform({ browser, scope })) {
        console.warn("Skipping dev platform fallback: WebCrypto is unavailable in this context")
        return platform
    }

    const fallback = await devplatform({ seed, runtime })
    return {
        ...fallback,
        ...platform,
        pair: platform.pair || fallback.pair,
        pub: platform.pub || fallback.pub
    }
}

export async function patchsiteplatform(site, options = {}) {
    const platform = await siteplatform(site, options)
    if (!site || typeof site !== "object") throw new Error("siteRequired")
    site.platform = platform
    return platform
}
