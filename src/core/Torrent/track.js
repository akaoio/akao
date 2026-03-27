function key(torrent) {
    return torrent?.infoHash || torrent?.magnetURI || torrent?.name
}

export function $track(torrent) {
    if (!torrent) return torrent

    const update = () => {
        const id = key(torrent)
        if (id) this.torrents.set(id, torrent)
        this.$emit("update", this.$snapshot(torrent))
    }

    update()
    if (this.bound.has(torrent)) return torrent
    this.bound.add(torrent)

    torrent.on("metadata", update)
    torrent.on("ready", update)
    torrent.on("done", () => {
        update()
        this.$emit("done", this.$snapshot(torrent))
    })
    torrent.on("warning", (warning) => this.$emit("warning", { torrent: this.$snapshot(torrent), warning }))
    torrent.on("error", (error) => this.$emit("error", { torrent: this.$snapshot(torrent), error }))
    torrent.on("download", (bytes) => this.$emit("download", { torrent: this.$snapshot(torrent), bytes }))
    torrent.on("upload", (bytes) => this.$emit("upload", { torrent: this.$snapshot(torrent), bytes }))
    torrent.on("wire", () => this.$emit("wire", this.$snapshot(torrent)))
    torrent.on("noPeers", (announceType) => {
        this.survive()
        this.$emit("noPeers", { torrent: this.$snapshot(torrent), announceType })
    })

    return torrent
}