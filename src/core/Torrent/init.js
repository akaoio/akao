import { BROWSER } from "../Utils/environment.js"

export async function $client() {
    if (this.client) return this.client
    if (this.ready) return this.ready

    this.ready = (async () => {
        const { default: WebTorrent } = BROWSER
            ? await import("./client.js")
            : await import("webtorrent")
        this.client = new WebTorrent(this.options)

        this.client.on("error", (error) => this.$emit("error", error))
        this.client.on("warning", (warning) => this.$emit("warning", warning))
        this.client.on("torrent", (torrent) => {
            this.$track(torrent)
            this.$emit("torrent", this.$snapshot(torrent))
        })

        return this.client
    })().catch((error) => {
        this.ready = null
        throw error
    })

    return this.ready
}

export async function init() {
    return this.$client()
}