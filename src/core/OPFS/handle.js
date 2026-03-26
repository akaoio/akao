// Resolve a file handle from an absolute path (relative to instance root).
// Pass create=true to create the file (and any missing parent directories).
export async function $handle(path = [], create = false) {
    const name = path.at(-1)
    const dir = await this.$dir(path.slice(0, -1), create)
    return dir.getFileHandle(name, { create })
}
