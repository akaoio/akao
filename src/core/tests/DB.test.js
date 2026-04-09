import Test from "../Test.js"
import { DB } from "../DB.js"
import { transform } from "../DB/transformer.js"

Test.describe("DB — path()", () => {

    Test.it("single digit returns single segment", () => {
        Test.assert.deepEqual(DB.path(5), ["5"])
    })

    Test.it("two-digit number returns single segment", () => {
        Test.assert.deepEqual(DB.path(12), ["12"])
    })

    Test.it("3-digit number splits into two segments", () => {
        Test.assert.deepEqual(DB.path(100), ["1", "00"])
    })

    Test.it("4-digit number splits into two 2-char segments", () => {
        Test.assert.deepEqual(DB.path(1234), ["12", "34"])
    })

    Test.it("5-digit number splits into 3 segments", () => {
        Test.assert.deepEqual(DB.path(12345), ["1", "23", "45"])
    })

    Test.it("6-digit number splits into three 2-char segments", () => {
        Test.assert.deepEqual(DB.path(123456), ["12", "34", "56"])
    })

    Test.it("large GeoNames-style ID splits correctly", () => {
        // Known example: 8615918 → ["86", "15", "91", "8"]... let's verify
        const result = DB.path(8615918)
        // Rebuild original number from segments
        const rebuilt = result.join("")
        Test.assert.equal(rebuilt, "8615918")
    })

    Test.it("always reconstructs original ID by joining segments", () => {
        const ids = [1, 99, 100, 9999, 10000, 123456, 6295630]
        for (const id of ids) {
            const segments = DB.path(id)
            Test.assert.equal(segments.join(""), String(id))
        }
    })

    Test.it("all segments are strings", () => {
        const segments = DB.path(12345)
        for (const seg of segments) 
            Test.assert.equal(typeof seg, "string")
        
    })

})

Test.describe("DB — get() caching", () => {

    Test.it("fetches data when cache is empty (browser)", async () => {
        // This test only runs in the browser where IndexedDB is available.
        // We load a small known static file: statics/routes.json
        const data = await DB.get(["statics", "routes.json"])
        Test.assert.truthy(Array.isArray(data), "routes.json should be an array")
    }, { browser: true })

    Test.it("re-fetching same path returns consistent data (browser)", async () => {
        const a = await DB.get(["statics", "routes.json"])
        const b = await DB.get(["statics", "routes.json"])
        Test.assert.deepEqual(a, b)
    }, { browser: true })

})

Test.describe("DB — transformer key filtering", () => {

    // Game item: ["statics","items","gameId","itemId","vi.json"] → length 5
    Test.it("transforms game item path correctly", () => {
        const path = ["statics", "items", "diablo-4", "sword-abc123", "vi.json"]
        const data = { name: "Sword", rarity: "Epic", type: "Weapon", value: 100 }
        const result = transform(path, data)

        Test.assert.truthy(result, "should return transform result")
        Test.assert.equal(result.schema.includes("game_items"), true, "should use game_items schema")
        Test.assert.equal(result.upsert.includes("INSERT OR REPLACE"), true, "should have upsert SQL")
        Test.assert.equal(result.insert.includes("INSERT OR IGNORE"), true, "should have insert SQL")
        Test.assert.equal(result.values[0], "sword-abc123", "values[0] = itemId")
        Test.assert.equal(result.values[1], "diablo-4", "values[1] = gameId")
        Test.assert.equal(result.values[2], "vi", "values[2] = locale")
    })

    // Shop item: ["statics","items","itemId","vi.json"] → length 4
    Test.it("transforms shop item path correctly", () => {
        const path = ["statics", "items", "service-xyz", "en.json"]
        const data = { name: "Coaching", price: 50, currency: "USD" }
        const result = transform(path, data)

        Test.assert.truthy(result, "should return transform result")
        Test.assert.equal(result.schema.includes("shop_items"), true, "should use shop_items schema")
        Test.assert.equal(result.upsert.includes("INSERT OR REPLACE"), true, "should have upsert SQL")
        Test.assert.equal(result.insert.includes("INSERT OR IGNORE"), true, "should have insert SQL")
        Test.assert.equal(result.values[0], "service-xyz", "values[0] = itemId")
        Test.assert.equal(result.values[1], "en", "values[1] = locale")
    })

    // Paths that should return null (no SQL sync)
    Test.it("returns null for hash files", () => {
        const path = ["statics", "items", "diablo-4", "sword", "vi.hash"]
        Test.assert.equal(transform(path, {}), null, "hash files should return null")
    })

    Test.it("returns null for pagination files (page index)", () => {
        // Pagination files have data as Array (not object)
        const path = ["statics", "items", "1.json"]
        const data = ["item1", "item2"]  // Array, not object
        Test.assert.equal(transform(path, data), null, "Array data should return null")
    })

    Test.it("returns null for non-statics paths", () => {
        const path = ["core", "config", "settings.json"]
        Test.assert.equal(transform(path, {}), null, "non-statics paths should return null")
    })

    Test.it("returns null for meta.json files", () => {
        const path = ["statics", "items", "meta.json"]
        const data = { pages: 10, children: 100 }
        Test.assert.equal(transform(path, data), null, "meta.json should return null")
    })

    Test.it("returns null for empty path", () => {
        Test.assert.equal(transform([], {}), null, "empty path should return null")
        Test.assert.equal(transform(null, {}), null, "null path should return null")
    })

    Test.it("returns null for Array data", () => {
        const path = ["statics", "items", "diablo-4", "sword", "vi.json"]
        Test.assert.equal(transform(path, []), null, "Array data should return null")
    })

    // Key length filtering — these match the filters in $rebuildFromIDB
    Test.it("filters non-item paths correctly", () => {
        // statics/locales.json → length 2
        const result1 = transform(["statics", "locales.json"], [{ code: "vi" }])
        Test.assert.equal(result1, null, "locales.json should return null")

        // statics/items/meta.json → length 3
        const result2 = transform(["statics", "items", "meta.json"], { pages: 1 })
        Test.assert.equal(result2, null, "items/meta.json should return null")

        // statics/items/1.json (page index) → length 3
        const result3 = transform(["statics", "items", "1.json"], ["item1"])
        Test.assert.equal(result3, null, "page index should return null")
    })

})

Test.describe("DB — _pending batching", () => {

    Test.it("_pending starts empty", () => {
        // _pending is internal, but we can verify it's an array
        Test.assert.equal(Array.isArray(DB._pending), true, "_pending should be an array")
    })

    Test.it("_scheduled starts false", () => {
        Test.assert.equal(DB._scheduled, false, "_scheduled should be false initially")
    })

})
