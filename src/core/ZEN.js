import { Statics } from "./Stores.js"

let BaseZENRuntime
let ZENRuntime
let zen = null
let load = null

let init = null

const chainWrappers = new WeakMap()

function isChainLike(value) {
    return !!value && typeof value === "object" && typeof value.get === "function" && typeof value.once === "function"
}

function wrapResult(value) {
    return isChainLike(value) ? wrapChain(value) : value
}

function onceValue(target, cb, opt) {
    if (typeof cb === "function") return target.once(cb, opt)
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
        target.once((data) => {
            if (settled) return
            settled = true
            clearTimeout(timeout)
            resolve(data)
        }, onceOpt)
    })
}

function wrapChain(chain) {
    if (!isChainLike(chain)) return chain
    if (chainWrappers.has(chain)) return chainWrappers.get(chain)
    const wrapped = new Proxy(chain, {
        get(target, prop, receiver) {
            if (prop === "once") return (cb, opt) => onceValue(target, cb, opt)
            const value = Reflect.get(target, prop, receiver)
            if (typeof value !== "function") return value
            return (...args) => wrapResult(value.apply(target, args))
        }
    })
    chainWrappers.set(chain, wrapped)
    return wrapped
}

function createRuntimeClass(Runtime) {
    return class AKAOZEN extends Runtime {
        chain() { return wrapChain(super.chain()) }
        get(...args) { return wrapChain(super.get(...args)) }
        put(...args) { return wrapResult(super.put(...args)) }
        on(...args) { return wrapResult(super.on(...args)) }
        once(cb, opt) { return onceValue(this._graph, cb, opt) }
        map(...args) { return wrapResult(super.map(...args)) }
        set(...args) { return wrapResult(super.set(...args)) }
        back(...args) { return wrapResult(super.back(...args)) }
    }
}

async function loadZEN() {
    if (zen && ZENRuntime) return true
    if (!load)
        load = (async () => {
            ;({ default: BaseZENRuntime } = await import("/core/ZEN/zen.min.js"))
            await import("/core/ZEN/lib/radix.js")
            await import("/core/ZEN/lib/radisk.js")
            await import("/core/ZEN/lib/opfs.js")
            await import("/core/ZEN/lib/rindexed.js")
            await import("/core/ZEN/lib/store.js")
            ZENRuntime = createRuntimeClass(BaseZENRuntime)
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
    return {
        peers: Statics.site?.peers || [],
        localStorage: false,
        radisk: true,
        file: "ZEN"
    }
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
