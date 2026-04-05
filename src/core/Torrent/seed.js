import { $save } from "./persist.js"

export async function seed(input, options = {}) {
    const client = await this.$client()

    const finalOptions = this._active && !options.announce ? { ...options, announce: [this._active] } : options

    return new Promise((resolve, reject) => {
        let seeded = null

        const onSeed = (torrent) => {
            seeded = torrent
            this.$track(torrent)
            torrent.off?.("error", onError)
            $save(torrent).catch(() => {})
            resolve(torrent)
        }

        const onError = (error) => {
            if (seeded) return
            reject(error instanceof Error ? error : new Error(String(error)))
        }

        const torrent = client.seed(input, finalOptions, onSeed)
        this.$track(torrent)
        torrent.on("error", onError)
    })
}