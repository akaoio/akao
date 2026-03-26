// List entry names (files and subdirectories) in a directory.
export async function list(path = []) {
    const handle = await this.$dir(this._path(path))
    const names = []
    for await (const name of handle.keys()) names.push(name)
    return names
}
