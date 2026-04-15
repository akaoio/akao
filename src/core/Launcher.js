import { threads } from "./Threads.js"
import { reset } from "./Utils/reset.js"
import { NODE, WIN } from "./Utils/environment.js"

globalThis.reset = reset
if (NODE && !globalThis._root) {
    let _root = decodeURIComponent(new URL("..", import.meta.url).pathname.replace(/\/$/, ""))
    // On Windows, URL.pathname yields /D:/... — strip the leading slash so Node fs calls work
    if (WIN && /^\/[A-Za-z]:/.test(_root)) _root = _root.slice(1)
    globalThis._root = _root
}

const PROFILES = {
    browser: [
        { name: "main", main: true, type: "module", path: "./threads/main.js" },
        { name: "update", worker: true, type: "module", path: "./threads/update.js" },
        { name: "discovery", worker: true, type: "module", path: "./threads/discovery.js" },
        { name: "wave", worker: true, type: "module", path: "./threads/wave.js" },
        { name: "qr", worker: true, type: "module", path: "./threads/qr.js" },
        { name: "sql", worker: true, type: "module", path: "./threads/sql.js" },
        { name: "onchain", worker: true, type: "module", path: "./threads/onchain.js" },
        { name: "offchain", worker: true, type: "module", path: "./threads/offchain.js" },
        { name: "torrent", worker: true, type: "module", path: "./threads/torrent.js" }
    ],
    headless: [
        { name: "main", main: true, type: "module", path: "./threads/headless.js" }
    ]
}

let started = null

export async function launch({ mode = NODE ? "headless" : "browser", profile } = {}) {
    if (started) return started

    started = (async () => {
        const registrations = {}
        const targets = profile || PROFILES[mode] || PROFILES.browser

        for (const { name, ...configs } of targets)
            registrations[name] = await threads.register(name, configs)

        const mainModule = registrations.main
        const main = mainModule?.default
        if (main?.ready) await main.ready

        const state = { mode, registrations, main, reset }
        globalThis.launcher = state
        return state
    })()

    return started
}

export const ready = globalThis._autolaunch === false ? Promise.resolve(null) : launch()
export { PROFILES }
