import { Chains, Wallets, Dexs, Statics } from "./Stores.js"
import { Context, getTheme, getFiat, getReferrer } from "./Context.js"
import { BROWSER } from "./Utils.js"
import { NODE } from "./Utils/environment.js"
import Router from "./Router.js"
import DB from "./DB.js"
import { patchsiteplatform } from "./Platform.js"

function isLoopbackHost(hostname = "") {
    return hostname === "localhost"
        || hostname === "127.0.0.1"
        || hostname === "::1"
        || /^\d+\.\d+\.\d+\.\d+$/.test(hostname)
}

function domainCandidates(hostname = "") {
    const value = String(hostname || "").toLowerCase().trim()
    if (!value || isLoopbackHost(value)) return ["localhost"]

    const labels = value.split(".").filter(Boolean)
    if (labels.length < 2) return [value]

    const candidates = []
    for (let i = 0; i <= labels.length - 2; i++) {
        const candidate = labels.slice(i).join(".")
        if (!candidates.includes(candidate)) candidates.push(candidate)
    }
    return candidates
}

export const Construct = {
    Site: async function () {
        const hostname = BROWSER ? globalThis.location?.hostname : "localhost"
        let domain = null

        for (const candidate of domainCandidates(hostname)) {
            const next = await DB.get(["statics", "domains", `${candidate}.json`])
            if (!next?.site) continue
            Statics.domain = candidate
            domain = next
            break
        }

        if (!domain?.site) {
            Statics.domain = "localhost"
            domain = await DB.get(["statics", "domains", "localhost.json"])
        }

        Statics.site = domain?.site ? await DB.get(["statics", "sites", domain.site, "configs.json"]) : null
        if (Statics.site) await patchsiteplatform(Statics.site)
        console.log("Constructed: Site")
        return !!Statics.site
    },
    GDB: async function () {
        if (!Statics.site) return
        if (BROWSER && !globalThis.crypto?.subtle) {
            globalThis.gun = null
            globalThis.Gun = null
            globalThis.SEA = null
            globalThis.sea = null
            console.warn("Skipping GDB: WebCrypto is unavailable in this context")
            return false
        }
        let GUN
        let SEA

        await import("./GDB.js")
        GUN = globalThis.GUN || globalThis.Gun
        SEA = globalThis.SEA || globalThis.Gun?.SEA
        const file = "GDB"
        const opfs = globalThis.ROPFS
        const gunOptions = {
            peers: Statics.site?.peers || [],
            localStorage: false,
            radisk: true,
            file
        }
        if (opfs?.supported?.({ file })) gunOptions.store = opfs({ file })

        globalThis.gun = GUN(gunOptions)
        globalThis.Gun = GUN
        globalThis.SEA = SEA
        globalThis.sea = SEA
        console.log("Constructed: GDB")
        return true
    },
    Chain: async function ({ id } = {}) {
        if (!id) return
        const { Chain } = await import("./Chain.js")
        const instance = new Chain({ id })
        if (Chains[instance.id]) return Chains[instance.id]
        if (typeof instance?.init === "function") await instance.init()
        Chains[instance.id] = instance
        return true
    },
    Chains: async function ({ chains = [] } = {}) {
        chains = Statics.site?.chains || chains
        if (!chains?.length) return
        for (const id of chains) await Construct.Chain({ id })
        console.log("Constructed: Chains")
        return true
    },
    Dex: async function ({ chain, dex, version } = {}) {
        if (!chain || !dex || !version) return
        const { Dex } = await import("./Dex.js")
        const instance = new Dex({ chain, dex, version })
        if (Dexs[instance.id]) return Dexs[instance.id]
        if (typeof instance?.init === "function") await instance.init()
        Dexs[instance.id] = instance
        return true
    },
    Dexs: async function ({ chains = [] } = {}) {
        chains = Statics.site?.chains || chains
        if (!chains?.length) return
        for (const id of chains) {
            const defis = await DB.get(["statics", "chains", id, "defis.json"])
            if (!defis?.length) continue
            for (const configs of defis) await Construct.Dex({ chain: id, ...configs })
        }
        console.log("Constructed: Dexs")
        return true
    },
    Wallet: async function ({ chain } = {}) {
        if (!chain) return
        const { Wallet } = await import("./Wallet.js")
        const instance = new Wallet({ chain })
        if (Wallets[instance.id]) return Wallets[instance.id]
        if (typeof instance?.init === "function") await instance.init()
        Wallets[instance.id] = instance
        return true
    },
    Wallets: async function () {
        for (const chain in Chains) await Construct.Wallet({ chain })
        console.log("Constructed: Wallets")
        return true
    },
    Torrent: async function () {
        const trackers = await DB.get(["statics", "trackers.json"])
        const { Torrent } = await import("./Torrent.js")
        const instance = new Torrent()
        instance.tracker(trackers || [])
        await instance.init()
        Statics.torrent = instance
        console.log("Constructed: Torrent")
        return true
    },
    Context: async function () {
        const router = Router.process()
        Context.set({
            ...router,
            theme: getTheme(),
            fiat: getFiat(),
            referrer: globalThis.localStorage ? globalThis.localStorage.getItem("referrer") : await getReferrer()
        })
        console.log("Constructed: Context")
        return true
    }
}
