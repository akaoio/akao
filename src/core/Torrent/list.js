export function list() {
    return [...this.torrents.values()].map((torrent) => this.$snapshot(torrent))
}