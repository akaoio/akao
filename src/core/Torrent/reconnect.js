const MAX = 5
const DELAY = 5000

export function reconnect() {
    if (!this.pool?.length) return this
    if ((this._reconnect || 0) >= MAX) return this
    this._reconnect = (this._reconnect || 0) + 1
    setTimeout(() => this.rotate(), DELAY)
    return this
}
