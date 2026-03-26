// Move (rename/relocate) a file.
// src and dst are path arrays relative to instance root.
export async function move(src, dst) {
    const full_src = this._path(src)
    const full_dst = this._path(dst)
    const handle = await this.$handle(full_src)
    const destDir = await this.$dir(full_dst.slice(0, -1), true)
    await handle.move(destDir, full_dst.at(-1))
}
