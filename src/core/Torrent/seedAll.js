import { FS } from "../FS.js"

// Directories under statics/ that should NOT be seeded to public P2P swarm.
// Add sensitive dir names here when going to production (e.g. "sites" if configs.torrent exist).
const SEED_BLACKLIST = []

// Recursively collect all .torrent file paths under a root path
async function collectTorrentFiles(path = []) {
    if (SEED_BLACKLIST.includes(path.at(-1))) return []
    const entries = await FS.dir(path)
    if (!entries) return []
    const results = []
    for (const entry of entries) {
        const entryPath = [...path, entry]
        if (await FS.isDirectory(entryPath)) {
            const nested = await collectTorrentFiles(entryPath)
            for (const p of nested) results.push(p)
        } else if (entry.endsWith(".torrent")) results.push(entryPath)
    }
    return results
}

/**
 * Seed all .torrent metadata files under statics/ into the given torrent client.
 * Shared between worker (browser) and main-thread (Node headless) seed loops.
 *
 * Yields to the event loop every BATCH_SIZE seeds so Node.js main-thread
 * mode doesn't starve tests/requests. In browser worker mode this yield
 * is harmless (no UI to protect inside a worker).
 */
const BATCH_SIZE = 50

export async function seedAll(torrent) {
    const files = await collectTorrentFiles(["statics"])
    for (let i = 0; i < files.length; i++) {
        const bytes = await FS.load(files[i])
        if (!bytes) continue
        torrent.seed(bytes, { announce: torrent.pool }).catch(() => {})
        if (i % BATCH_SIZE === 0) await new Promise(r => setTimeout(r, 0))
    }
}
