// Create a directory (and any missing parent segments).
// Returns the FileSystemDirectoryHandle.
export async function mkdir(path) {
    return this.$dir(this._path(path), true)
}
