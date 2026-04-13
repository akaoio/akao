

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
import { FS } from "./FS.js"
import { Indexes } from "./Stores.js"
import { BROWSER, DEV } from "./Utils/environment.js"
import { transform } from "./DB/transformer.js"

export class DB {
    static _db = null
    static _schemas = new Set()  // tracks which table schemas have been initialized this session
    static _pending = []         // pending $syncToSQL ops, flushed as one batch per microtask
    static _scheduled = false    // whether a flush microtask is already scheduled

    // Fallback chain for requestIdleCallback — yields to the browser scheduler,
    // then MessageChannel, then setTimeout as a last resort.
    static _yield() {
        if (typeof scheduler !== "undefined" && scheduler.yield) return scheduler.yield()

        if (typeof MessageChannel !== "undefined")
            return new Promise((resolve) => {
                const ch = new MessageChannel()
                ch.port1.onmessage = resolve
                ch.port2.postMessage(null)
            })

        return new Promise((r) => setTimeout(r, 0))
    }

    static async _hash(path = []) {
        if (!(BROWSER && typeof fetch === "function")) {
            const hash = await FS.load(path, { quiet: true })
            return typeof hash === "string"
                ? { ok: true, status: 200, hash }
                : { ok: false, status: null, hash: undefined }
        }

        try {
            const url = Array.isArray(path) ? `/${path.filter(Boolean).join("/")}` : String(path ?? "")
            const response = await fetch(url)
            if (response.ok) return { ok: true, status: response.status, hash: await response.text() }
            if (response.status === 404) return { ok: false, status: 404, hash: undefined }
            return { ok: false, status: response.status, hash: undefined }
        } catch {
            return { ok: false, status: null, hash: undefined }
        }
    }

    static async get(path = []) {
        let type = path.at?.(-1)?.endsWith?.(".hash") ? "hash" : "data"

        // Dev mode: skip hash validation, always load fresh — but still index IDB + SQL
        if (DEV) {
            if (type === "hash") return FS.load(path)
            const data = await FS.load(path, { fresh: true, quiet: true })
            if (typeof data !== "undefined") {
                await Indexes.Statics.get(path).put(data)
                DB.$syncToSQL(path, data)
            } else {
                await Indexes.Statics.del(path)
                await Indexes.Hashes.del(path)
                DB.$syncdel(path)
            }
            return data
        }

        const memory = await Indexes.Hashes.get(path).once()
        const hashpth = path?.with?.(-1, path?.at?.(-1)?.replace?.(/\.\w+$/, ".hash"))
        const hashres = await DB._hash(hashpth)
        const hash = hashres.hash
        if (memory && hashres.ok && memory === hash) {
            if (type === "hash") return hash
            const cached = await Indexes.Statics.get(path).once()
            DB._syncInsert(path, cached)
            return cached
        }
        if (hashres.ok) await Indexes.Hashes.get(path).put(hash)
        if (type === "hash") return hash
        const data = await FS.load(path)
        if (typeof data !== "undefined") {
            await Indexes.Statics.get(path).put(data)
            DB.$syncToSQL(path, data)
        } else if (memory && hashres.status === 404) {
            await Indexes.Hashes.del(path)
            await Indexes.Statics.del(path)
            DB.$syncdel(path)
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
                DB._yield().then(() => DB.$rebuildFromIDB())
                return db
            })().catch((err) => {
                DB._db = null // allow retry on next call instead of permanently failing
                throw err
            })

        return DB._db
    }

    // Rebuilds SQL from IDB cache on startup.
    // Runs entirely in idle time — browser can interrupt between chunks.
    // Uses INSERT OR IGNORE so items already in SQL are skipped instantly (index
    // lookup only, no write). Only truly missing items get inserted.
    // This handles cold start, partial data, and OPFS-cleared scenarios.
    static async $rebuildFromIDB() {
        const allKeys = await Indexes.Statics.keys()
        if (!allKeys.length) return

        // Filter to only item locale keys (game: statics/items/gameId/itemId/locale → length 5,
        // shop: statics/items/itemId/locale → length 4). Meta.json and pagination files have
        // different lengths and will be filtered out naturally. We also validate using transform()
        // to ensure we're only picking up actual item paths.
        const itemKeys = allKeys.filter(k => {
            if (!Array.isArray(k) || k.length < 4) return false
            // Validate using transform - only actual item locale paths return non-null
            const op = transform(k, {})
            return op !== null
        })
        if (!itemKeys.length) return

        // Count guard: fast path — skip rebuild if SQL already has enough items.
        // Wrap in try-catch in case tables don't exist yet (fresh DB).
        const db = await DB.sql()
        let gameCount = 0, shopCount = 0
        try {
            const [gameRow] = await db.all("SELECT COUNT(*) as n FROM game_items")
            const [shopRow] = await db.all("SELECT COUNT(*) as n FROM shop_items")
            gameCount = gameRow?.n ?? 0
            shopCount = shopRow?.n ?? 0
        } catch {
            // Tables don't exist yet — proceed with rebuild
        }
        const needsRebuild = gameCount < itemKeys.filter((k) => k.length === 5).length || shopCount < itemKeys.filter((k) => k.length === 4).length

        if (!needsRebuild) {
            DB._yield().then(() => DB.$syncCatalog())
            return
        }

        // SQL is missing items — process in chunks via idle callbacks.
        // Each chunk reads 30 keys from IDB in parallel then syncs via INSERT OR IGNORE.
        const CHUNK = 30
        let i = 0

        const processChunk = async () => {
            const chunk = itemKeys.slice(i, i + CHUNK)
            i += chunk.length
            try {
                const entries = await Promise.all(
                    chunk.map(async (path) => ({ path, data: await Indexes.Statics.$get(path) }))
                )
                for (const { path, data } of entries) if (data) DB._syncInsert(path, data)
                
            } catch (err) {
                console.warn("[DB.$rebuildFromIDB] chunk error:", err.message)
            }
            if (i < itemKeys.length)
                DB._yield().then(processChunk)
            else
                DB._yield().then(() => DB.$syncCatalog())
        }

        DB._yield().then(processChunk)
    }

    // Proactively syncs the full item catalog (all items, all locales) into IDB + SQL.
    // Runs after $rebuildFromIDB, entirely in idle time via _yield().
    // Each item goes through DB.get() — hash validation ensures freshness, IDB gets
    // populated, $syncToSQL fires automatically. Subsequent runs are cheap: hash match
    // → served from IDB, only 1 lightweight .hash request per item.
    static async $syncCatalog() {
        // Load available locales from static catalog via DB.get() for consistent
        // hash validation and IDB caching behavior (same as rest of the app).
        const localeList = await DB.get(["statics", "locales.json"])
        if (!Array.isArray(localeList)) return
        const locales = localeList.map(l => l.code).filter(Boolean)
        if (!locales.length) return

        // Load pagination metadata to know total items and pages
        const meta = await DB.get(["statics", "items", "meta.json"])
        if (!meta?.pages || !meta?.children) return

        const db = await DB.sql()

        // Count guard: skip if SQL already has items for all locales
        // Wrap in try-catch in case tables don't exist yet (fresh DB before first flush).
        let sqlTotal = 0
        try {
            const [gameRow] = await db.all("SELECT COUNT(*) as n FROM game_items")
            const [shopRow] = await db.all("SELECT COUNT(*) as n FROM shop_items")
            sqlTotal = (gameRow?.n ?? 0) + (shopRow?.n ?? 0)
        } catch {
            // Tables don't exist yet — proceed with sync
        }
        if (sqlTotal >= meta.children * locales.length) return

        // Walk each pagination page in idle time, fetch all items for all locales.
        // DB.get() handles: hash check → IDB cache → $syncToSQL → SQL/OPFS.
        for (let page = 1; page <= meta.pages; page++) {
            await DB._yield()
            const keys = await DB.get(["statics", "items", `${page}.json`])
            if (!Array.isArray(keys)) continue

            for (const key of keys) {
                const parts = key.split("/")
                for (const locale of locales) {
                    await DB._yield()
                    const path = parts.length === 2
                        ? ["statics", "items", parts[0], parts[1], `${locale}.json`]
                        : ["statics", "items", parts[0], `${locale}.json`]
                    await DB.get(path)
                }
            }
        }
    }

    // Like $syncToSQL but uses INSERT OR IGNORE — safe to call for items already in SQL.
    // _flush reads op.sql to distinguish which variant to execute.
    static _syncInsert(path, data) {
        const op = transform(path, data)
        if (!op) return
        DB._pending.push({ schema: op.schema, sql: op.insert, values: op.values })
        if (!DB._scheduled) {
            DB._scheduled = true
            queueMicrotask(() => DB._flush())
        }
    }

    static query(sql, params = []) {
        return DB.sql().then(db => db.all(sql, params))
    }

    static $syncToSQL(path, data) {
        const op = transform(path, data)
        if (!op) return
        DB._pending.push(op)
        if (!DB._scheduled) {
            DB._scheduled = true
            queueMicrotask(() => DB._flush())
        }
    }

    static $syncdel(path) {
        const op = transform(path, null)
        if (!op?.delete) return
        DB._pending.push({ schema: op.schema, sql: op.delete, values: op.values })
        if (!DB._scheduled) {
            DB._scheduled = true
            queueMicrotask(() => DB._flush())
        }
    }

    // Flush all pending ops as a single batch transaction.
    // Called once per microtask — collects everything that arrived in the same tick.
    // If the batch fails (bad data in one op), falls back to individual inserts so
    // good ops are not lost. Bad ops are skipped with a warning.
    static _flush() {
        DB._scheduled = false
        const ops = DB._pending.splice(0)
        if (!ops.length) return

        DB.sql()
            .then(async db => {
                // Ensure all required schemas exist before inserting
                const newSchemas = [...new Set(ops.map(op => op.schema))]
                    .filter(s => !DB._schemas.has(s))
                for (const schema of newSchemas) {
                    await db.exec(schema)
                    DB._schemas.add(schema)
                }

                // Fast path: all ops in one transaction (1 fsync)
                // op.sql is set by _syncInsert (INSERT OR IGNORE),
                // op.upsert is set by $syncToSQL (INSERT OR REPLACE).
                const toSQL = op => op.sql ?? op.upsert
                try {
                    return await db.batch(ops.map(op => ({ sql: toSQL(op), params: op.values })))
                } catch {
                    // Slow fallback: insert individually to isolate the bad op
                    for (const op of ops)
                        try {
                            await db.run(toSQL(op), op.values)
                        } catch (err) {
                            console.warn("[DB._flush] skipped:", err.message, op.values?.[0])
                        }
                    
                }
            })
            .catch(err => console.warn("[DB._flush]", err.message))
    }

    static path(id) {
        const str = String(id)
        const segments = []
        for (let i = str.length; i > 0; i -= 2) segments.unshift(str.slice(Math.max(0, i - 2), i))
        return segments
    }
}

export default DB
