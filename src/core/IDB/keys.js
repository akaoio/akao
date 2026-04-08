import { BROWSER } from "../Utils.js"

// Returns all keys stored in this IDB store.
// Browser: uses getAllKeys() — returns the raw keys (path arrays for Statics).
// Node:    returns empty — in dev mode DB.get() always syncs SQL directly, no rebuild needed.
export async function keys() {
    await this.ready
    if (!BROWSER) return []
    const req = await this.execute({ operation: (store) => store.getAllKeys() })
    return req.result ?? []
}
