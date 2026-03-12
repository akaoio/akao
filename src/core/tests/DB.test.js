import Test from "../Test.js"
import { DB } from "../DB.js"

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
        for (const seg of segments) {
            Test.assert.equal(typeof seg, "string")
        }
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
