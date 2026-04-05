import Thread from "/core/Thread.js"
import { Construct } from "/core/Construct.js"
import { FS } from "/core/FS.js"
import { Statics } from "/core/Stores.js"
import { loop } from "/core/Utils.js"

const thread = new Thread()

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

async function seedAll(torrent) {
    const files = await collectTorrentFiles(["statics"])
    for (const filePath of files) {
        const bytes = await FS.load(filePath)
        if (!bytes) continue
        torrent.seed(bytes, { announce: torrent.pool }).catch(() => {})
    }
}

thread.init = async function () {
    // TODO: check p2p_optout from localStorage before starting.
    await Construct.Torrent()
    const torrent = Statics.torrent
    if (!torrent) return

    await seedAll(torrent)

    loop({
        process: () => seedAll(torrent),
        delay: 3600000
    })
}

// TODO: method disable()
// thread.disable = async function () {
//     const torrent = Statics.torrent
//     if (!torrent) return
//     await torrent.destroy()
//     Statics.torrent = null
// }
