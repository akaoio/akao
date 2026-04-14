import { Progress } from "../Progress.js"
import { Statics } from "../Stores.js"
import { merge } from "../Utils.js"
import { Construct } from "../Construct.js"
import Thread from "../Thread.js"
import DB from "../DB.js"

const thread = new Thread()

function makestore() {
    const memory = new Map()
    return {
        get length() { return memory.size },
        key(index) { return [...memory.keys()][index] ?? null },
        getItem(key) { return memory.has(key) ? memory.get(key) : null },
        setItem(key, value) { memory.set(String(key), String(value)) },
        removeItem(key) { memory.delete(key) },
        clear() { memory.clear() }
    }
}

function setloc(url = "http://localhost/") {
    const next = new URL(url, "http://localhost/")
    const current = globalThis.location || {}
    Object.assign(current, {
        href: next.href,
        origin: next.origin,
        protocol: next.protocol,
        host: next.host,
        hostname: next.hostname,
        port: next.port,
        pathname: next.pathname,
        search: next.search,
        hash: next.hash
    })
    globalThis.location = current
}

function sethist() {
    if (globalThis.history) return
    globalThis.history = {
        state: null,
        pushState(state, _title, url) {
            this.state = state
            if (url) setloc(String(url))
        }
    }
}

function setenv() {
    if (!globalThis.localStorage) globalThis.localStorage = makestore()
    if (!globalThis.sessionStorage) globalThis.sessionStorage = makestore()
    if (!globalThis.location) setloc()
    sethist()
}

Progress.set({
    Site: false,
    Chains: false,
    Dexs: false,
    Wallets: false,
    ZEN: false,
    Context: false
})

thread.init = async function () {
    setenv()
    const { site } = Statics
    if (!site) throw new Error("No site configs found during preload")

    merge(Statics, {
        routes: await DB.get(["statics", "routes.json"]),
        locales: await DB.get(["statics", "locales.json"]),
        fiats: await DB.get(["statics", "fiats.json"]),
        themes: await DB.get(["statics", "themes.json"])
    })

    Progress.set({ Site: true })
    Progress.set({ Chains: await Construct.Chains() })
    Progress.set({ Dexs: await Construct.Dexs() })
    Progress.set({ Wallets: await Construct.Wallets() })
    Progress.set({ ZEN: await Construct.ZEN() })
    Progress.set({ Context: await Construct.Context() })

    globalThis.headless = true
}

export default thread
