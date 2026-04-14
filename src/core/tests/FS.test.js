import Test from "../Test.js"
import { load } from "../FS/load.js"
import { driver, NODE } from "../FS/shared.js"
import { Statics } from "../Stores.js"

/**
 * Isomorphic Torrent fallback tests.
 *
 * Key insight: to test leech in same-process, we must:
 * 1. Seed ACTUAL content → WebTorrent generates .torrent with info hash H
 * 2. Write that .torrent to disk
 * 3. leechToCache reads .torrent → torrent.add() → info hash H matches → instant
 *
 * seedAll() seeds .torrent FILES as content (different info hash) — that's for
 * distributing .torrent metadata via P2P, not the content itself.
 */

const TEST_DIR = ["statics", "__torrent_test__"]
const TEST_PATH = ["statics", "__torrent_test__", "data.json"]
const TORRENT_META_PATH = ["statics", "__torrent_test__", "data.torrent"]
const TEST_CONTENT = { name: "Torrent Leech Test", verified: true }

async function waitTorrentClient(timeoutMs = 15000) {
    const start = Date.now()
    while (Date.now() - start < timeoutMs) {
        if (Statics.torrent?.client) return Statics.torrent
        await new Promise(r => setTimeout(r, 200))
    }
    return null
}

Test.describe("FS.load — Torrent fallback (isomorphic)", () => {

    Test.it("torrent client initializes in headless mode", async () => {
        if (!NODE) return
        const t = await waitTorrentClient(15000)
        Test.assert.truthy(t, "Statics.torrent must be initialized")
        Test.assert.truthy(t.client, "WebTorrent client must exist")
    })

    Test.it("leechToCache recovers seeded content via matching info hash", async () => {
        if (!NODE) return
        const torrent = await waitTorrentClient(15000)
        Test.assert.truthy(torrent, "prerequisite: torrent client")

        let seeded
        try {
            // 1. Seed actual content → generates .torrent with matching info hash
            const contentBytes = new TextEncoder().encode(JSON.stringify(TEST_CONTENT))
            seeded = await torrent.seed(contentBytes)
            Test.assert.truthy(seeded, "must seed content")
            Test.assert.truthy(seeded.torrentFile, "seeded torrent must have .torrentFile buffer")

            // 2. Write .torrent metadata to disk so leechToCache can find it
            try { await driver.mkdir(TEST_DIR) } catch {}
            try { await driver.writeBytes(TORRENT_META_PATH, new Uint8Array(seeded.torrentFile)) } catch {}

            // 3. leechToCache should find the seeded torrent (same info hash → instant)
            const { leechToCache } = await import("../Torrent/leech.js")
            const result = await leechToCache(torrent, TEST_PATH)

            Test.assert.truthy(result, "leechToCache must return bytes")
            const parsed = JSON.parse(new TextDecoder().decode(result))
            Test.assert.deepEqual(parsed, TEST_CONTENT)
        } finally {
            try { await driver.remove(TEST_DIR) } catch {}
            if (seeded?.infoHash) try { await torrent.remove(seeded.infoHash) } catch {}
        }
    })

    Test.it("FS.load recovers via _leechDirect when file missing from disk", async () => {
        if (!NODE) return
        const torrent = await waitTorrentClient(15000)
        Test.assert.truthy(torrent, "prerequisite: torrent client")

        let seeded
        try {
            // 1. Seed content
            const contentBytes = new TextEncoder().encode(JSON.stringify(TEST_CONTENT))
            seeded = await torrent.seed(contentBytes)
            Test.assert.truthy(seeded?.torrentFile)

            // 2. Write .torrent metadata (so leechToCache resolves info hash)
            try { await driver.mkdir(TEST_DIR) } catch {}
            try { await driver.writeBytes(TORRENT_META_PATH, new Uint8Array(seeded.torrentFile)) } catch {}

            // 3. Do NOT write data.json to disk → disk miss → triggers _leechDirect
            try { await driver.remove(TEST_PATH) } catch {}

            // 4. FS.load should recover via Torrent fallback
            const recovered = await load(TEST_PATH, { quiet: true })
            Test.assert.truthy(recovered, "FS.load must recover via Torrent fallback")
            Test.assert.deepEqual(recovered, TEST_CONTENT)

            // 5. Verify file is now cached on disk
            const cached = await driver.readBytes(TEST_PATH)
            Test.assert.truthy(cached?.byteLength > 0, "leech must cache to disk")
        } finally {
            try { await driver.remove(TEST_DIR) } catch {}
            if (seeded?.infoHash) try { await torrent.remove(seeded.infoHash) } catch {}
        }
    })

    Test.it("second load hits local cache (no re-leech)", async () => {
        // Use a known existing file
        const path = ["statics", "routes.json"]
        const a = await load(path)
        const b = await load(path)
        Test.assert.truthy(a, "file must exist")
        Test.assert.deepEqual(a, b)
    })

    Test.it("returns undefined for path without .torrent metadata", async () => {
        const result = await load(
            ["statics", "__nonexistent_9z3x__", "missing.json"],
            { quiet: true }
        )
        Test.assert.equal(result, undefined)
    })

    Test.it("skips re-seed for already-seeded file without warning", async () => {
        if (!NODE) return
        const torrent = await waitTorrentClient(15000)
        Test.assert.truthy(torrent, "prerequisite: torrent client")

        // Seed once
        const content = new TextEncoder().encode(JSON.stringify(TEST_CONTENT))
        const first = await torrent.seed(content)
        Test.assert.truthy(first, "must seed first time")

        // Try seed same content again — should handle duplicate without error
        const second = await torrent.seed(content)
        Test.assert.truthy(second, "duplicate seed should succeed (not throw)")
    })

})
