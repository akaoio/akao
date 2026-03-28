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

    Test.it("tracker classifies URLs by scheme", () => {
        const t = new Torrent()
        t.tracker([
            "http://tracker.example.com/announce",
            "https://tracker.example.com/announce",
            "ws://tracker.example.com/announce",
            "wss://tracker.example.com/announce"
        ])
        Test.assert.equal(t.pool.length, 4)
        Test.assert.equal(t.trackers.http.length, 1)
        Test.assert.equal(t.trackers.https.length, 1)
        Test.assert.equal(t.trackers.ws.length, 1)
        Test.assert.equal(t.trackers.wss.length, 1)
    })

    Test.it("tracker sets active after calling with pool", () => {
        const t = new Torrent()
        t.tracker(["wss://tracker.example.com/announce"])
        Test.assert.equal(t._active, "wss://tracker.example.com/announce")
        Test.assert.equal(t._scheme, "wss")
    })

    Test.it("tracker accepts a single URL string", () => {
        const t = new Torrent()
        t.tracker("ws://tracker.example.com/announce")
        Test.assert.equal(t.pool.length, 1)
        Test.assert.equal(t._active, "ws://tracker.example.com/announce")
    })

    Test.it("tracker resets reconnect counter", () => {
        const t = new Torrent()
        t._reconnect = 3
        t.tracker(["wss://tracker.example.com/announce"])
        Test.assert.equal(t._reconnect, 0)
    })

    Test.it("rotate picks a different tracker when multiple are available", () => {
        const t = new Torrent()
        t.tracker([
            "ws://tracker1.example.com/announce",
            "wss://tracker2.example.com/announce"
        ])
        const first = t._active
        t.rotate()
        Test.assert.notEqual(t._active, first)
    })

    Test.it("rotate emits tracker event", () => {
        const t = new Torrent()
        t.tracker([
            "ws://a.example.com/announce",
            "wss://b.example.com/announce"
        ])
        let emitted = null
        t.on("tracker", (payload) => { emitted = payload })
        t.rotate()
        Test.assert.truthy(emitted)
        Test.assert.truthy(emitted.active)
        Test.assert.truthy(emitted.scheme)
        Test.assert.equal(emitted.pool.length, 2)
    })

    Test.it("rotate prefers cross-scheme fallback", () => {
        const t = new Torrent()
        t.tracker([
            "ws://a.example.com/announce",
            "wss://b.example.com/announce",
            "wss://c.example.com/announce"
        ])
        // Force active to a ws URL
        t._active = "ws://a.example.com/announce"
        t._scheme = "ws"
        t.rotate()
        // Should switch to wss (cross-scheme)
        Test.assert.equal(t._scheme, "wss")
    })

    Test.it("rotate is a no-op when pool is empty", () => {
        const t = new Torrent()
        t.rotate()
        Test.assert.equal(t._active, null)
    })

    Test.it("reconnect increments counter and is bounded by MAX", () => {
        const t = new Torrent()
        t.tracker(["wss://tracker.example.com/announce"])
        t._reconnect = 4
        t.reconnect()
        Test.assert.equal(t._reconnect, 5)
        // At MAX, reconnect should not increment further
        t.reconnect()
        Test.assert.equal(t._reconnect, 5)
    })

    Test.it("reconnect is a no-op when pool is empty", () => {
        const t = new Torrent()
        t.reconnect()
        Test.assert.equal(t._reconnect, 0)
    })

})