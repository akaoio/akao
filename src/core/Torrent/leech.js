import { driver } from "../FS/shared.js"

/**
 * Leech a file from P2P and cache it to local storage.
 * Shared logic used by:
 *   - Worker mode (browser):     threads/torrent.js → thread.leech
 *   - Main-thread mode (Node):   FS/load.js → _leechDirect
 *
 * Handles the case where the torrent is already seeded in-process
 * (WebTorrent throws "duplicate torrent" on add() — we fall back to .get()).
 *
 * @param {Object} torrentInstance - Live Torrent class instance with .add() / .get()
 * @param {string[]} path - Content file path (e.g. ["statics","items","x","en.json"])
 * @param {number} timeoutMs - Max time to wait for piece download
 * @returns {Promise<Uint8Array|null>} File bytes on success, null on failure
 */
export async function leechToCache(torrentInstance, path, timeoutMs = 10000) {
    if (!torrentInstance || !Array.isArray(path) || !path.length) return null
    const last = path.at(-1)
    if (!last || !last.includes(".")) return null

    // Build .torrent metadata path: vi.json → vi.torrent
    const torrentPath = [...path]
    torrentPath[torrentPath.length - 1] = last.replace(/\.\w+$/, ".torrent")

    let torrentBytes
    try { torrentBytes = await driver.readBytes(torrentPath) } catch { return null }
    if (!torrentBytes) return null

    // Try add() first (download from peers).
    // WebTorrent throws if already seeded in-process:
    //   v1: "Cannot add duplicate torrent <hex40>"
    //   v2: "A torrent with the same id is already being seeded"
    // In that case, look up the existing torrent.
    let t
    try {
        t = await torrentInstance.add(torrentBytes)
    } catch (e) {
        const hash = e?.message?.match(/([0-9a-f]{40})/)?.[1]
        if (hash) {
            t = torrentInstance.get(hash)
        } else {
            // Fallback: find existing torrent by file name
            const client = torrentInstance.client
            t = client?.torrents?.find(tr => tr.name === last)
        }
        if (!t) return null
    }

    return _extractAndCache(t, path, last, timeoutMs)
}

async function _extractAndCache(t, path, fileName, timeoutMs) {
    return new Promise((resolve) => {
        let settled = false
        const cleanup = () => {
            clearTimeout(timeout)
            t.off?.("done", onDone)
            t.off?.("error", onError)
        }
        const finish = (value) => {
            if (settled) return
            settled = true
            cleanup()
            resolve(value)
        }
        const timeout = setTimeout(() => finish(null), timeoutMs)

        const onDone = async () => {
            if (settled) return
            try {
                const file = t.files?.find(f => f.name === fileName) || t.files?.[0]
                if (!file) { finish(null); return }
                const blob = await (file.blob?.() || file.getBlob?.())
                if (!blob) { finish(null); return }
                const bytes = new Uint8Array(await blob.arrayBuffer())
                try { await driver.writeBytes(path, bytes) } catch {}
                finish(bytes)
            } catch (e) {
                console.debug("[leech] extract failed:", e?.message)
                finish(null)
            }
        }

        const onError = (e) => {
            console.debug("[leech] torrent error:", e?.message)
            finish(null)
        }

        if (t.done) { onDone(); return }
        t.once("done", onDone)
        t.once("error", onError)
    })
}
