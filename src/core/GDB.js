import "./GDB/gun.js"
import "./GDB/sea.js"
import "./GDB/lib/radix.js"
import "./GDB/lib/radisk.js"
import "./GDB/lib/opfs.js"
import "./GDB/lib/rindexed.js"
import "./GDB/lib/store.js"
import "./GDB/lib/pen.js"
import { Statics } from "./Stores.js"

export const GUN = globalThis.Gun
export const SEA = globalThis.SEA || globalThis.Gun?.SEA

// Evaluated after Construct.Site() — Statics.site is available at this point
const file = "GDB"
const opfs = globalThis.ROPFS
const gunOptions = {
    peers: Statics.site?.peers || [],
    localStorage: false,
    radisk: true,
    file
}
if (opfs?.supported?.({ file })) gunOptions.store = opfs({ file })

const gun = GUN(gunOptions)
globalThis.gun = gun
globalThis.Gun = GUN
globalThis.SEA = SEA
globalThis.sea = SEA

export default gun
