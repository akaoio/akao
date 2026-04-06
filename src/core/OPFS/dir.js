// Navigate from the instance root to a subdirectory path (internal).
// Pass create=true to create missing segments.
export async function $dir(path = [], create = false) {
    let handle = await this.$root()
    for (const segment of path) handle = await handle.getDirectoryHandle(segment, { create })

    return handle
}

// List entry names (files and subdirectories) in a directory (public).
export async function dir(path = []) {
    const handle = await this.$dir(this._path(path))
    const names = []
    for await (const name of handle.keys()) names.push(name)
    return names
}
