import Thread from "/core/Thread.js"
import { Construct } from "/core/Construct.js"
import { Statics } from "/core/Stores.js"
import { leech } from "/core/Torrent/leech.js"
import { driver } from "/core/FS/shared.js"

const thread = new Thread()

thread.init = async function () {
    await Construct.Torrent()
}

/**
 * Observability: report current torrent client state.
 * Used by tests and debug console to verify seeding/tracker health.
 */
thread.status = function () {
    const t = Statics.torrent
    if (!t) return { ready: false, seeded: 0, tracker: null, scheme: null }
    return {
        ready: true,
        seeded: t.torrents?.size ?? 0,
        tracker: t._active ?? null,
        scheme: t._scheme ?? null,
        pool: t.pool?.length ?? 0
    }
}

/**
 * Leech a file from P2P and cache it to OPFS.
 * Called from main thread via threads.queue({ thread: "torrent", method: "leech", params: { path } }).
 * Returns { success: true } if file was leeched and cached.
 */
thread.leech = async function ({ path }) {
    const bytes = await leech(Statics.torrent, path)
    return { success: !!bytes }
}

/**
 * Seed content from OPFS. Called after HTTP fetch caches content.
 * Reads content from OPFS, seeds it, saves generated .torrent metadata.
 */
thread.seed = async function ({ path }) {
    const torrent = Statics.torrent
    if (!torrent || !Array.isArray(path) || !path.length) return { success: false }
    const last = path.at(-1)
    if (!last || !last.includes(".")) return { success: false }
    // Skip metadata files — seeding them writes wrong .torrent metadata
    if (last.endsWith(".hash") || last.endsWith(".torrent")) return { success: true }

    // Note: we don't pre-check by name — files like meta.json exist in many dirs
    // and name-only check would skip valid seeds. WebTorrent handles duplicates
    // via "same id" error → treat as success.
    const contentBytes = await driver.readBytes(path).catch(() => null)
    if (!contentBytes) return { success: false }

    const torrentPath = [...path]
    torrentPath[torrentPath.length - 1] = last.replace(/\.\w+$/, ".torrent")

    try {
        const t = await torrent.seed(contentBytes, { name: last, announce: torrent.pool })
        if (t.torrentFile)
            await driver.writeBytes(torrentPath, new Uint8Array(t.torrentFile)).catch(() => {})

        return { success: true }
    } catch (e) {
        console.debug("[torrent.seed] error:", e?.message)
        return { success: false }
    }
}

