

/**
 * DB - Hash-validated static file loader
 *
 * Production: validates cache freshness via .hash files before serving cached data.
 * Dev mode (localhost): skips hash validation, always loads fresh files directly.
 *
 * Production flow:
 *   1. Check Indexes.Hashes for the cached hash of the requested path
 *   2. Load the `.hash` file from the server (lightweight, always fresh)
 *   3. If hashes match → serve from Indexes.Statics (no re-fetch)
 *   4. If hashes differ → re-fetch data, update both Indexes.Statics and Indexes.Hashes
 */
import { load } from "./FS.js"
import { Indexes } from "./Stores.js"
import { DEV } from "./Utils/environment.js"

export class DB {
    static async get(path = []) {
        // Dev mode: skip hash validation, always load fresh
        if (DEV) return load(path)

        let type = path.at?.(-1)?.endsWith?.(".hash") ? "hash" : "data"
        const memory = await Indexes.Hashes.get(path).once()
        const hash = await load(path?.with?.(-1, path?.at?.(-1)?.replace?.(/\.\w+$/, ".hash")))
        if (memory && hash && memory === hash) return type === "hash" ? hash : await Indexes.Statics.get(path).once()
        if (hash) await Indexes.Hashes.get(path).put(hash)
        if (type === "hash") return hash
        const data = await load(path)
        if (typeof data !== "undefined") await Indexes.Statics.get(path).put(data)
        return data
    }

    static path(id) {
        const str = String(id)
        const segments = []
        for (let i = str.length; i > 0; i -= 2) segments.unshift(str.slice(Math.max(0, i - 2), i))
        return segments
    }
}

export default DB
