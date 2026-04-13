import "./GDB/gun.js"
import "./GDB/sea.js"
import "./GDB/lib/radix.js"
import "./GDB/lib/radisk.js"
import "./GDB/lib/opfs.js"
import "./GDB/lib/rindexed.js"
import "./GDB/lib/store.js"
import "./GDB/lib/pen.js"

export const GUN = globalThis.Gun
export const SEA = globalThis.SEA

// Lazy proxy — delegates to globalThis.gun after Construct.GDB() initializes it
const gun = new Proxy({}, { get(_, prop) { return globalThis.gun?.[prop] } })
export default gun
