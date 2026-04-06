import { FS } from "../FS.js"

const STATE_ROOT = ["torrent", "state"]

export async function $save(torrent) {
    const id = torrent?.infoHash
    if (!id) return
    const metadata = {
        infoHash: id,
        magnetURI: torrent.magnetURI,
        name: torrent.name,
        addedAt: Date.now(),
    }
    await FS.write([...STATE_ROOT, `${id}.json`], metadata)
}

export async function $load() {
    if (!(await FS.exist(STATE_ROOT))) return []
    const files = await FS.dir(STATE_ROOT)
    const results = []
    for (const f of files) {
        if (!f.endsWith(".json")) continue
        const data = await FS.load([...STATE_ROOT, f])
        if (data?.infoHash) results.push(data)
    }
    return results
}

export async function $delete(infoHash) {
    await FS.remove([...STATE_ROOT, `${infoHash}.json`])
}
