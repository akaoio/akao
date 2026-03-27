export function rotate() {
    if (!this.pool?.length) return this

    const others = this.pool.filter((url) => url !== this._active)
    const candidates = others.length ? others : this.pool

    // Prefer a different scheme for cross-transport fallback
    const crossScheme = candidates.filter((url) => url.split("://")[0] !== this._scheme)
    const from = crossScheme.length ? crossScheme : candidates

    const next = from[Math.floor(Math.random() * from.length)]
    if (!next) return this

    this._active = next
    this._scheme = next.split("://")[0] || null

    this.$emit("tracker", { active: this._active, scheme: this._scheme, pool: [...this.pool] })

    return this
}
