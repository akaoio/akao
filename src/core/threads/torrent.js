import Thread from "/core/Thread.js"
import { Construct } from "/core/Construct.js"
import { Statics } from "/core/Stores.js"
import { loop } from "/core/Utils.js"
import { seedAll } from "/core/Torrent/seedAll.js"
import { leechToCache } from "/core/Torrent/leech.js"

const thread = new Thread()

thread.init = async function () {
    await Construct.Torrent()
    const torrent = Statics.torrent
    if (!torrent) return

    await seedAll(torrent)

    loop({
        process: () => seedAll(torrent),
        delay: 3600000
    })
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
    const bytes = await leechToCache(Statics.torrent, path)
    return { success: !!bytes }
}
