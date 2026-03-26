// Navigate from the instance root to a subdirectory path.
// Pass create=true to create missing segments.
export async function $dir(path = [], create = false) {
    let handle = await this.$root()
    for (const segment of path) {
        handle = await handle.getDirectoryHandle(segment, { create })
    }
    return handle
}
