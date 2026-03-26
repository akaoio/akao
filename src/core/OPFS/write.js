// Write data to a file, creating it (and any parent directories) if needed.
// data can be string, ArrayBuffer, TypedArray, Blob, or any WritableStream-compatible value.
export async function write(path, data) {
    const handle = await this.$handle(this._path(path), true)
    const writable = await handle.createWritable()
    await writable.write(data)
    await writable.close()
}
