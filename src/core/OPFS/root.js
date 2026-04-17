// Module-level cached OPFS origin handle — shared across all OPFS instances
let _origin = null

export function supportsOPFS(scope = globalThis) {
    return typeof scope?.navigator?.storage?.getDirectory === "function"
}

export async function $root() {
    if (!supportsOPFS()) throw new Error("OPFSUnavailable")
    if (!_origin) _origin = await navigator.storage.getDirectory()
    return _origin
}
