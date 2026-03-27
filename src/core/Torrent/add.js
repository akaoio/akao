export async function add(input, options = {}) {
    const client = await this.$client()

    const finalOptions = this._active && !options.announce ? { ...options, announce: [this._active] } : options

    return new Promise((resolve, reject) => {
        let settled = false
        const torrent = client.add(input, finalOptions)
        this.$track(torrent)

        const cleanup = () => {
            torrent.off?.("ready", onReady)
            torrent.off?.("metadata", onReady)
            torrent.off?.("error", onError)
        }

        const onReady = () => {
            if (settled) return
            settled = true
            cleanup()
            resolve(torrent)
        }

        const onError = (error) => {
            if (settled) return
            settled = true
            cleanup()
            reject(error instanceof Error ? error : new Error(String(error)))
        }

        if (torrent.ready || torrent.infoHash) {
            onReady()
            return
        }

        torrent.on("ready", onReady)
        torrent.on("metadata", onReady)
        torrent.on("error", onError)
    })
}