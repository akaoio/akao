export async function remove(id, options = {}) {
    const client = await this.$client()

    return new Promise((resolve, reject) => {
        client.remove(id, options, (error) => {
            if (error) {
                reject(error instanceof Error ? error : new Error(String(error)))
                return
            }

            this.torrents.delete(id)
            for (const [key, torrent] of this.torrents)
                if (torrent?.infoHash === id || torrent?.magnetURI === id) this.torrents.delete(key)

            resolve(true)
        })
    })
}