import { BROWSER } from "../Utils.js"

// Returns all keys stored in this IDB store.
// Browser: uses getAllKeys() — returns the raw keys (path arrays for Statics).
// Node:    returns empty — the Node IDB backend does not use OPFS/IDB for persistence
//          and does not require rebuild, so keys enumeration is unnecessary.
export async function keys() {
    await this.ready
    if (!BROWSER) return []
    const req = await this.execute({ operation: (store) => store.getAllKeys() })
    return req.result ?? []
}
