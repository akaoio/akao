function read(metric) {
    if (typeof metric === "function") return metric.call(this)
    return metric || 0
}

export function $snapshot(torrent) {
    if (!torrent) return null
    return {
        name: torrent.name,
        infoHash: torrent.infoHash,
        magnetURI: torrent.magnetURI,
        progress: torrent.progress || 0,
        downloaded: torrent.downloaded || 0,
        uploaded: torrent.uploaded || 0,
        downloadSpeed: read(torrent.downloadSpeed),
        uploadSpeed: read(torrent.uploadSpeed),
        numPeers: torrent.numPeers || 0,
        ready: !!torrent.ready,
        done: !!torrent.done,
        paused: !!torrent.paused,
        length: torrent.length || 0
    }
}