import { Statics } from "./Stores.js"

let GUN
let ZENRuntime
let zen = null
let load = null

const root = typeof window !== "undefined"
    ? window
    : typeof self !== "undefined"
        ? self
        : undefined

let init = null

async function loadZEN() {
    if (zen && GUN && ZENRuntime) return true
    if (!load)
        load = (async () => {
            try {
                ;({ default: GUN } = await import("./ZEN/gun.js"))
                await import("./ZEN/sea.js")
                await import("./ZEN/lib/radix.js")
                await import("./ZEN/lib/radisk.js")
                await import("./ZEN/lib/opfs.js")
                await import("./ZEN/lib/rindexed.js")
                await import("./ZEN/lib/store.js")
                await import("./ZEN/src/pen.js")
                ;({ default: ZENRuntime } = await import("./ZEN/zen.js"))
            } catch {
                ;({ default: GUN } = await import("@akaoio/zen/gun.js"))
                await import("@akaoio/zen/sea.js")
                await import("@akaoio/zen/lib/radix.js")
                await import("@akaoio/zen/lib/radisk.js")
                await import("@akaoio/zen/lib/opfs.js")
                await import("@akaoio/zen/lib/rindexed.js")
                await import("@akaoio/zen/lib/store.js")
                await import("@akaoio/zen/src/pen.js")
                ;({ default: ZENRuntime } = await import("@akaoio/zen/zen.js"))
            }
            zen = new ZENRuntime()
            return true
        })().catch((error) => {
            load = null
            throw error
        })
    await load
    return true
}

function options() {
    const file = "ZEN"
    const opfs = root?.ROPFS
    const opt = {
        peers: Statics.site?.peers || [],
        localStorage: false,
        radisk: true,
        file
    }
    if (opfs?.supported?.({ file })) opt.store = opfs({ file })
    return opt
}

export async function initZEN() {
    await loadZEN()
    const runtime = zen
    if (runtime?._graphInstance) return true
    if (!init)
        init = Promise.resolve()
            .then(() => {
                runtime.use(GUN(options()))
                return true
            })
            .catch((error) => {
                init = null
                throw error
            })
    await init
    return true
}

export function userSoul(pub) {
    if (!pub) throw new Error("pubRequired")
    return `~${pub}`
}

export async function once(node, timeoutMs = 800) {
    return await new Promise((resolve) => {
        const timeout = setTimeout(() => resolve(undefined), timeoutMs)
        node.once((data) => {
            clearTimeout(timeout)
            resolve(data)
        })
    })
}

export { zen as default }
