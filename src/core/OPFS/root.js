// Module-level cached OPFS origin handle — shared across all OPFS instances
let _origin = null

export async function $root() {
    if (!_origin) _origin = await navigator.storage.getDirectory()
    return _origin
}
