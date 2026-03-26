import Test from "../Test.js"
import SQL from "../SQL.js"

let _counter = 0
const _run = Date.now()
function dbName() { return `__test_sql_${_run}_${_counter++}` }

// ─── open / ready ─────────────────────────────────────────────────────────────

Test.describe("SQL — open + ready", () => {

    Test.it("ready resolves for a fresh database", async () => {
        const db = new SQL({ name: dbName() })
        const result = await db.ready
        Test.assert.truthy(result !== undefined)
    }, { browser: true })

})

// ─── exec ─────────────────────────────────────────────────────────────────────

Test.describe("SQL — exec", () => {

    Test.it("CREATE TABLE returns empty array", async () => {
        const db = new SQL({ name: dbName() })
        await db.ready
        const rows = await db.exec("CREATE TABLE t (id INTEGER PRIMARY KEY, val TEXT)")
        Test.assert.deepEqual(rows, [])
    }, { browser: true })

    Test.it("SELECT returns rows as objects", async () => {
        const db = new SQL({ name: dbName() })
        await db.ready
        await db.exec("CREATE TABLE t (id INTEGER PRIMARY KEY, val TEXT)")
        await db.exec("INSERT INTO t VALUES (1, 'hello')")
        const rows = await db.exec("SELECT * FROM t")
        Test.assert.equal(rows.length, 1)
        Test.assert.equal(rows[0].id, 1)
        Test.assert.equal(rows[0].val, "hello")
    }, { browser: true })

    Test.it("parameterised query binds values correctly", async () => {
        const db = new SQL({ name: dbName() })
        await db.ready
        await db.exec("CREATE TABLE t (id INTEGER PRIMARY KEY, val TEXT)")
        await db.exec("INSERT INTO t VALUES (?, ?)", [42, "world"])
        const rows = await db.exec("SELECT val FROM t WHERE id = ?", [42])
        Test.assert.equal(rows[0].val, "world")
    }, { browser: true })

})

// ─── run ──────────────────────────────────────────────────────────────────────

Test.describe("SQL — run", () => {

    Test.it("INSERT returns {changes, lastId}", async () => {
        const db = new SQL({ name: dbName() })
        await db.ready
        await db.exec("CREATE TABLE t (id INTEGER PRIMARY KEY, val TEXT)")
        const result = await db.run("INSERT INTO t VALUES (1, 'a')")
        Test.assert.equal(result.changes, 1)
        Test.assert.equal(result.lastId, 1)
    }, { browser: true })

    Test.it("UPDATE reports correct change count", async () => {
        const db = new SQL({ name: dbName() })
        await db.ready
        await db.exec("CREATE TABLE t (id INTEGER PRIMARY KEY, val TEXT)")
        await db.exec("INSERT INTO t VALUES (1, 'a')")
        await db.exec("INSERT INTO t VALUES (2, 'b')")
        const result = await db.run("UPDATE t SET val = 'x'")
        Test.assert.equal(result.changes, 2)
    }, { browser: true })

})

// ─── get ──────────────────────────────────────────────────────────────────────

Test.describe("SQL — get", () => {

    Test.it("returns first matching row", async () => {
        const db = new SQL({ name: dbName() })
        await db.ready
        await db.exec("CREATE TABLE t (id INTEGER PRIMARY KEY, val TEXT)")
        await db.exec("INSERT INTO t VALUES (1, 'first')")
        await db.exec("INSERT INTO t VALUES (2, 'second')")
        const row = await db.get("SELECT * FROM t ORDER BY id")
        Test.assert.equal(row.id, 1)
        Test.assert.equal(row.val, "first")
    }, { browser: true })

    Test.it("returns null when no rows match", async () => {
        const db = new SQL({ name: dbName() })
        await db.ready
        await db.exec("CREATE TABLE t (id INTEGER PRIMARY KEY, val TEXT)")
        const row = await db.get("SELECT * FROM t WHERE id = 999")
        Test.assert.equal(row, null)
    }, { browser: true })

})

// ─── all ──────────────────────────────────────────────────────────────────────

Test.describe("SQL — all", () => {

    Test.it("returns all matching rows", async () => {
        const db = new SQL({ name: dbName() })
        await db.ready
        await db.exec("CREATE TABLE t (id INTEGER PRIMARY KEY, val TEXT)")
        await db.exec("INSERT INTO t VALUES (1, 'a')")
        await db.exec("INSERT INTO t VALUES (2, 'b')")
        await db.exec("INSERT INTO t VALUES (3, 'c')")
        const rows = await db.all("SELECT * FROM t ORDER BY id")
        Test.assert.equal(rows.length, 3)
        Test.assert.equal(rows[2].val, "c")
    }, { browser: true })

    Test.it("returns empty array when no rows match", async () => {
        const db = new SQL({ name: dbName() })
        await db.ready
        await db.exec("CREATE TABLE t (id INTEGER PRIMARY KEY)")
        const rows = await db.all("SELECT * FROM t WHERE id > 100")
        Test.assert.deepEqual(rows, [])
    }, { browser: true })

})

// ─── batch ────────────────────────────────────────────────────────────────────

Test.describe("SQL — batch", () => {

    Test.it("executes multiple statements and returns results", async () => {
        const db = new SQL({ name: dbName() })
        await db.ready
        await db.exec("CREATE TABLE t (id INTEGER PRIMARY KEY, val TEXT)")
        const results = await db.batch([
            { sql: "INSERT INTO t VALUES (1, 'x')" },
            { sql: "INSERT INTO t VALUES (2, 'y')" },
            { sql: "SELECT * FROM t ORDER BY id" },
        ])
        Test.assert.equal(results[2].length, 2)
        Test.assert.equal(results[2][0].val, "x")
    }, { browser: true })

})

// ─── error handling ───────────────────────────────────────────────────────────

Test.describe("SQL — error handling", () => {

    Test.it("rejects on invalid SQL", async () => {
        const db = new SQL({ name: dbName() })
        await db.ready
        await Test.assert.rejects(db.exec("NOT VALID SQL !!!"))
    }, { browser: true })

    Test.it("rejects on constraint violation", async () => {
        const db = new SQL({ name: dbName() })
        await db.ready
        await db.exec("CREATE TABLE t (id INTEGER PRIMARY KEY)")
        await db.exec("INSERT INTO t VALUES (1)")
        await Test.assert.rejects(db.exec("INSERT INTO t VALUES (1)"))
    }, { browser: true })

})
