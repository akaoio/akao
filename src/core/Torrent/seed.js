export async function seed(input, options = {}) {
    const client = await this.$client()

    return new Promise((resolve, reject) => {
        let seeded = null

        const onSeed = (torrent) => {
            seeded = torrent
            this.$track(torrent)
            torrent.off?.("error", onError)
            resolve(torrent)
        }

        const onError = (error) => {
            if (seeded) return
            reject(error instanceof Error ? error : new Error(String(error)))
        }

        const torrent = client.seed(input, options, onSeed)
        this.$track(torrent)
        torrent.on("error", onError)
    })
}