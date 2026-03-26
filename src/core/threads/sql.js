import Thread from "/core/Thread.js"

const thread = new Thread()

// SQLite WASM module — loaded once on first use
let sqlite3 = null

// Active DB connections: name → sqlite3.oo1.OpfsDb instance
const dbs = new Map()

// Pending write counters for periodic WAL checkpoint: name → count
const pending = new Map()

// Checkpoint interval handles: name → id
const intervals = new Map()

const FLUSH_INTERVAL = 2000       // ms between periodic checkpoints
const CHECKPOINT_THRESHOLD = 200  // force checkpoint after this many writes

async function ensureSQLite() {
    if (sqlite3) return sqlite3
    const { default: sqlite3InitModule } = await import("/core/SQL/sqlite3.js")
    // Use absolute URL so that sub-workers spawned by the OPFS proxy can resolve the path
    const base = self.location.origin
    sqlite3 = await sqlite3InitModule({
        locateFile: (file) => `${base}/core/SQL/${file}`,
        print: () => {},
        printErr: () => {}
    })
    return sqlite3
}

function db(name) {
    const d = dbs.get(name)
    if (!d) throw new Error(`Database not open: ${name}`)
    return d
}

function trackWrite(name) {
    const count = (pending.get(name) || 0) + 1
    pending.set(name, count)
    if (count >= CHECKPOINT_THRESHOLD) {
        const d = dbs.get(name)
        if (d) d.exec("PRAGMA wal_checkpoint(PASSIVE)")
        pending.set(name, 0)
    }
}

// ── Thread lifecycle ──────────────────────────────────────────────────────────

thread.init = async function () {
    await ensureSQLite()
}

// ── DB management ─────────────────────────────────────────────────────────────

thread.open = async function ({ db: name }) {
    await ensureSQLite()
    if (dbs.has(name)) return { ok: true }

    if (!sqlite3.oo1?.OpfsDb) throw new Error("OPFS VFS not available — must run in a dedicated Worker")

    const d = new sqlite3.oo1.OpfsDb(`/${name}.db`)
    // WAL mode: writes go to the WAL file; we flush on our own schedule
    d.exec("PRAGMA journal_mode = WAL")
    // NORMAL: fsync only at checkpoints, not after every commit
    d.exec("PRAGMA synchronous = NORMAL")
    // Disable automatic checkpoint — we drive it manually
    d.exec("PRAGMA wal_autocheckpoint = 0")

    dbs.set(name, d)
    pending.set(name, 0)

    // Periodic WAL flush
    const id = setInterval(() => {
        const d = dbs.get(name)
        if (!d || !pending.get(name)) return
        d.exec("PRAGMA wal_checkpoint(PASSIVE)")
        pending.set(name, 0)
    }, FLUSH_INTERVAL)
    intervals.set(name, id)

    return { ok: true }
}

thread.close = function ({ db: name }) {
    const d = dbs.get(name)
    if (!d) return { ok: true }
    clearInterval(intervals.get(name))
    intervals.delete(name)
    pending.delete(name)
    d.exec("PRAGMA wal_checkpoint(FULL)")
    d.close()
    dbs.delete(name)
    return { ok: true }
}

// ── Query handlers ────────────────────────────────────────────────────────────

// Execute any SQL; returns result rows for SELECT, empty array for DML.
thread.exec = function ({ db: name, sql, params }) {
    const d = db(name)
    const rows = []
    d.exec({ sql, bind: params || [], rowMode: "object", callback: (row) => rows.push(row) })
    return rows
}

// Write statement — returns { changes, lastId }.
thread.run = function ({ db: name, sql, params }) {
    const d = db(name)
    d.exec({ sql, bind: params || [] })
    trackWrite(name)
    return {
        changes: d.changes(),
        lastId: d.selectValue("SELECT last_insert_rowid()")
    }
}

// SELECT first row or null.
thread.get = function ({ db: name, sql, params }) {
    const d = db(name)
    const rows = []
    d.exec({ sql, bind: params || [], rowMode: "object", callback: (row) => rows.push(row) })
    return rows[0] ?? null
}

// SELECT all rows.
thread.all = function ({ db: name, sql, params }) {
    const d = db(name)
    const rows = []
    d.exec({ sql, bind: params || [], rowMode: "object", callback: (row) => rows.push(row) })
    return rows
}

// Run multiple statements in one transaction.
// queries: [{ sql, params }, ...]
thread.batch = function ({ db: name, queries }) {
    const d = db(name)
    const results = []
    d.transaction(() => {
        for (const { sql, params } of queries) {
            const rows = []
            d.exec({ sql, bind: params || [], rowMode: "object", callback: (row) => rows.push(row) })
            results.push(rows)
        }
    })
    trackWrite(name)
    return results
}
