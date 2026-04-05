import { $delete } from "./persist.js"

export async function remove(id, options = {}) {
    const client = await this.$client()

    // Resolve infoHash before WebTorrent removes the torrent from its internal state
    let infoHash = typeof id === "string" && /^[0-9a-f]{40}$/i.test(id) ? id : null
    if (!infoHash)
        for (const [, torrent] of this.torrents)
            if (torrent?.infoHash === id || torrent?.magnetURI === id) {
                infoHash = torrent.infoHash
                break
            }

    return new Promise((resolve, reject) => {
        client.remove(id, options, (error) => {
            if (error) {
                reject(error instanceof Error ? error : new Error(String(error)))
                return
            }

            this.torrents.delete(id)
            for (const [key, torrent] of this.torrents) if (torrent?.infoHash === id || torrent?.magnetURI === id) this.torrents.delete(key)

            if (infoHash) $delete(infoHash).catch(() => {})
            resolve(true)
        })
    })
}
