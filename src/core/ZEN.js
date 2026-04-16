import { Statics } from "./Stores.js"

let GUN
let ZENRuntime
let zen = null
let load = null
let chainOncePatched = false

const root = typeof window !== "undefined"
    ? window
    : typeof self !== "undefined"
        ? self
        : undefined

let init = null

function patchChainOnce() {
    if (chainOncePatched || !GUN?.chain?.once) return
    const nativeOnce = GUN.chain.once
    GUN.chain.once = function (cb, opt) {
        if (typeof cb === "function") return nativeOnce.call(this, cb, opt)
        const timeoutMs = typeof cb === "number"
            ? cb
            : typeof opt === "number"
                ? opt
                : typeof cb?.timeout === "number"
                    ? cb.timeout
                    : typeof opt?.timeout === "number"
                        ? opt.timeout
                        : 800
        const onceOpt = typeof cb === "object" && cb && typeof cb !== "function"
            ? cb
            : typeof opt === "object" && opt
                ? opt
                : undefined
        return new Promise((resolve) => {
            let settled = false
            const timeout = setTimeout(() => {
                if (settled) return
                settled = true
                resolve(undefined)
            }, timeoutMs)
            nativeOnce.call(this, (data) => {
                if (settled) return
                settled = true
                clearTimeout(timeout)
                resolve(data)
            }, onceOpt)
        })
    }
    GUN.chain.once.native = nativeOnce
    chainOncePatched = true
}

async function loadZEN() {
    if (zen && GUN && ZENRuntime) return true
    if (!load)
        load = (async () => {
            ;({ default: GUN } = await import("@akaoio/zen/gun.js"))
            await import("@akaoio/zen/sea.js")
            await import("@akaoio/zen/lib/radix.js")
            await import("@akaoio/zen/lib/radisk.js")
            await import("@akaoio/zen/lib/opfs.js")
            await import("@akaoio/zen/lib/rindexed.js")
            await import("@akaoio/zen/lib/store.js")
            await import("@akaoio/zen/src/pen.js")
            ;({ default: ZENRuntime } = await import("@akaoio/zen/zen.js"))
            patchChainOnce()
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

export { zen as default }
