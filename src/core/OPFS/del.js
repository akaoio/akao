// Delete a file or directory (recursive).
export async function del(path) {
    const full = this._path(path)
    const parent = await this.$dir(full.slice(0, -1))
    await parent.removeEntry(full.at(-1), { recursive: true })
}
