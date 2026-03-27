const SCHEMES = ["http", "https", "ws", "wss"]

export function tracker(urls = []) {
    if (!Array.isArray(urls)) urls = [urls]
    this.pool = urls.filter(Boolean)
    this.trackers = { http: [], https: [], ws: [], wss: [] }

    for (const url of this.pool) {
        const scheme = url.split("://")[0]
        if (SCHEMES.includes(scheme)) this.trackers[scheme].push(url)
    }

    this._survive = 0

    if (!this._active || !this.pool.includes(this._active)) this.rotate()

    return this
}
