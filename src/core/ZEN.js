import { Statics } from "./Stores.js"
import BaseZENRuntime from "./ZEN/zen.min.js"
import "./ZEN/lib/radix.js"
import "./ZEN/lib/radisk.js"
import "./ZEN/lib/opfs.js"
import "./ZEN/lib/rindexed.js"
import "./ZEN/lib/store.js"
import "./ZEN/lib/then.js"

const zen = new BaseZENRuntime()
let init = null

function options() {
    const $options = {
        peers: Statics.site?.peers || [],
        localStorage: false,
        radisk: true,
        file: "ZEN"
    }
    // If OPFS is supported and available, use it for storage instead of IndexedDB
    try {
        if (typeof navigator !== "undefined" && navigator?.storage?.getDirectory) $options.opfs = true
    } catch {}
    return $options
}

export async function initZEN() {
    if (zen?._graphInstance) return true
    if (!init)
        init = Promise.resolve()
            .then(() => {
                zen.use(BaseZENRuntime.graph.create(options()))
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
