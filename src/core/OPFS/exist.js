// Check whether a file or directory exists at path.
export async function exist(path) {
    try {
        await this.$handle(this._path(path))
        return true
    } catch {
        try {
            await this.$dir(this._path(path))
            return true
        } catch {
            return false
        }
    }
}
