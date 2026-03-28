

/**
 * DB - Hash-validated static file loader
 *
 * Production: validates cache freshness via .hash files before serving cached data.
 * Dev mode (localhost): skips hash validation, always loads fresh — but still writes
 *   to IDB and syncs to SQL so the DB stays populated for query development.
 *
 * Production flow:
 *   1. Check Indexes.Hashes for the cached hash of the requested path
 *   2. Load the `.hash` file from the server (lightweight, always fresh)
 *   3. If hashes match → serve from Indexes.Statics (no re-fetch)
 *   4. If hashes differ → re-fetch data, update both Indexes.Statics and Indexes.Hashes
 *      → fire-and-forget $syncToSQL: write to SQLite without blocking the return
 */
import { load } from "./FS.js"
import { Indexes } from "./Stores.js"
import { DEV } from "./Utils/environment.js"
import { transform, ALL_TABLES } from "./DB/transformer.js"

export class DB {
    static _db = null
    static _schemas = new Set()  // tracks which table schemas have been initialized this session

    static async get(path = []) {
        let type = path.at?.(-1)?.endsWith?.(".hash") ? "hash" : "data"

        // Dev mode: skip hash validation, always load fresh — but still index IDB + SQL
        if (DEV) {
            if (type === "hash") return load(path)
            const data = await load(path)
            if (typeof data !== "undefined") {
                await Indexes.Statics.get(path).put(data)
                DB.$syncToSQL(path, data)
            }
            return data
        }

        const memory = await Indexes.Hashes.get(path).once()
        const hash = await load(path?.with?.(-1, path?.at?.(-1)?.replace?.(/\.\w+$/, ".hash")))
        if (memory && hash && memory === hash) return type === "hash" ? hash : await Indexes.Statics.get(path).once()
        if (hash) await Indexes.Hashes.get(path).put(hash)
        if (type === "hash") return hash
        const data = await load(path)
        if (typeof data !== "undefined") {
            await Indexes.Statics.get(path).put(data)
            DB.$syncToSQL(path, data)
        }
        return data
    }

    // Lazy singleton — stores the Promise itself, not the resolved instance.
    // This prevents race conditions: concurrent calls all await the same Promise
    // instead of each spawning their own SQL instance and migration.
    // SQL.js uses browser-only absolute paths (/core/Threads.js),
    // so we import it dynamically to avoid pulling it into non-browser environments.
    static sql() {
        if (!DB._db)
            DB._db = (async () => {
                const { default: SQL } = await import("./SQL.js")
                const db = new SQL({ name: "shop" })
                await db.ready
                await db.exec(ALL_TABLES.map((t) => `DROP TABLE IF EXISTS ${t};`).join("\n"))
                DB._schemas.clear()
                return db
            })().catch((err) => {
                DB._db = null // allow retry on next call instead of permanently failing
                throw err
            })
        
        return DB._db
    }

    static query(sql, params = []) {
        return DB.sql().then(db => db.all(sql, params))
    }

    static $syncToSQL(path, data) {
        const op = transform(path, data)
        if (!op) return
        DB.sql()
            .then(db => {
                const ready = DB._schemas.has(op.schema)
                    ? Promise.resolve()
                    : db.exec(op.schema).then(() => DB._schemas.add(op.schema))
                return ready.then(() => db.run(op.upsert, op.values))
            })
            .catch(err => console.warn("[DB.$syncToSQL]", path, err.message))
    }

    static path(id) {
        const str = String(id)
        const segments = []
        for (let i = str.length; i > 0; i -= 2) segments.unshift(str.slice(Math.max(0, i - 2), i))
        return segments
    }
}

export default DB
