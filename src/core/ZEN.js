import { Statics } from "./Stores.js"

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
    const chain = ZENRuntime?.graph?.chain
    if (chainOncePatched || !chain?.once) return
    const nativeOnce = chain.once
    chain.once = function (cb, opt) {
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
    chain.once.native = nativeOnce
    chainOncePatched = true
}

async function loadZEN() {
    if (zen && ZENRuntime) return true
    if (!load)
        load = (async () => {
            ;({ default: ZENRuntime } = await import("/core/ZEN/zen.js"))
            await import("/core/ZEN/lib/radix.js")
            await import("/core/ZEN/lib/radisk.js")
            await import("/core/ZEN/lib/opfs.js")
            await import("/core/ZEN/lib/rindexed.js")
            await import("/core/ZEN/lib/store.js")
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
                runtime.use(ZENRuntime.graph.create(options()))
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
