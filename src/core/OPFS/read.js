// Read a file and return its contents as an ArrayBuffer.
export async function read(path) {
    const handle = await this.$handle(this._path(path))
    const file = await handle.getFile()
    return file.arrayBuffer()
}
