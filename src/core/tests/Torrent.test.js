import Test from "../Test.js"
import Torrent from "../Torrent.js"

function fakeTorrent(overrides = {}) {
    return {
        name: "demo",
        infoHash: "abc123",
        magnetURI: "magnet:?xt=urn:btih:abc123",
        progress: 0.5,
        downloaded: 50,
        uploaded: 20,
        downloadSpeed: 10,
        uploadSpeed: 5,
        numPeers: 2,
        ready: true,
        done: false,
        paused: false,
        length: 100,
        ...overrides
    }
}

Test.describe("Torrent", () => {

    Test.it("lists tracked torrent snapshots", () => {
        const torrent = new Torrent()
        torrent.torrents.set("abc123", fakeTorrent())

        const list = torrent.list()
        Test.assert.equal(list.length, 1)
        Test.assert.equal(list[0].infoHash, "abc123")
        Test.assert.equal(list[0].numPeers, 2)
    })

    Test.it("returns tracked torrent instances by id", () => {
        const torrent = new Torrent()
        const tracked = fakeTorrent()
        torrent.torrents.set(tracked.infoHash, tracked)

        Test.assert.equal(torrent.get(tracked.infoHash), tracked)
    })

    Test.it("destroy is idempotent before client initialization", async () => {
        const torrent = new Torrent()
        const result = await torrent.destroy()
        Test.assert.truthy(result)
    })

})