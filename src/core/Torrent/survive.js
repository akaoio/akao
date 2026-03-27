const MAX = 5
const DELAY = 5000

export function survive() {
    if (!this.pool?.length) return this
    if ((this._survive || 0) >= MAX) return this
    this._survive = (this._survive || 0) + 1
    setTimeout(() => this.rotate(), DELAY)
    return this
}
