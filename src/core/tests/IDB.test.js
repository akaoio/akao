import Test from "../Test.js"
import IDB from "../IDB.js"
import { NODE } from "../Utils.js"

// Use a unique DB name per test run to avoid cross-test contamination
let _counter = 0
function dbName() { return `__test_idb_${_counter++}` }

Test.describe("IDB — get / put / del", () => {

    Test.it("put and get round-trip a value", async () => {
        const db = new IDB({ name: dbName() })
        await db.get("key").put("value")
        const result = await db.get("key").once()
        Test.assert.equal(result, "value")
    })

    Test.it("put and get with object value", async () => {
        const db = new IDB({ name: dbName() })
        await db.get("obj").put({ a: 1, b: 2 })
        const result = await db.get("obj").once()
        Test.assert.deepEqual(result, { a: 1, b: 2 })
    })

    Test.it("get returns undefined for missing key", async () => {
        const db = new IDB({ name: dbName() })
        const result = await db.get("missing").once()
        Test.assert.equal(result, undefined)
    })

    Test.it("del removes a key", async () => {
        const db = new IDB({ name: dbName() })
        await db.get("x").put(42)
        await db.get("x").del()
        const result = await db.get("x").once()
        Test.assert.equal(result, undefined)
    })

    Test.it("overwrites existing value", async () => {
        const db = new IDB({ name: dbName() })
        await db.get("v").put(1)
        await db.get("v").put(2)
        const result = await db.get("v").once()
        Test.assert.equal(result, 2)
    })

    Test.it("independent stores do not share data", async () => {
        const a = new IDB({ name: dbName() })
        const b = new IDB({ name: dbName() })
        await a.get("k").put("from-a")
        const inB = await b.get("k").once()
        Test.assert.equal(inB, undefined)
    })

    Test.it("chained get builds nested path (Node in-memory)", async () => {
        if (!NODE) return // path semantics differ for browser IDB keys
        const db = new IDB({ name: dbName() })
        await db.get("parent").get("child").put("nested-value")
        const result = await db.get("parent").get("child").once()
        Test.assert.equal(result, "nested-value")
    })

    Test.it("watch notifies when value changes", async () => {
        const db = new IDB({ name: dbName() })
        let received = null
        const off = await db.get("watched").on((val) => { received = val })
        await db.get("watched").put("new-value")
        // Small delay to allow async notification
        await new Promise(r => setTimeout(r, 10))
        Test.assert.equal(received, "new-value")
        if (off) off()
    })

})
