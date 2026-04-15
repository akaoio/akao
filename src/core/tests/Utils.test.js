import Test from "../Test.js"
import {
    sha256,
    base64UrlToHex, hexToBase64Url,
    formatNumber, beautifyNumber, formatBalance, shorten,
    randomInt, randomText, randomItem,
    spintax, encodeQuery, clone, merge,
    evaluate,
    NODE, BROWSER
} from "../Utils.js"

Test.describe("Utils — Environment", () => {

    Test.it("exactly one of NODE/BROWSER is true", () => {
        Test.assert.truthy(
            (NODE && !BROWSER) || (!NODE && BROWSER),
            "exactly one of NODE or BROWSER must be true"
        )
    })

})

Test.describe("Utils — sha256", () => {

    Test.it("hashes empty string to known value", () => {
        Test.assert.equal(
            sha256(""),
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"
        )
    })

    Test.it("hashes 'hello' to known value", () => {
        Test.assert.equal(
            sha256("hello"),
            "2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824"
        )
    })

    Test.it("same input always produces same hash", () => {
        Test.assert.equal(sha256("abc"), sha256("abc"))
    })

    Test.it("different inputs produce different hashes", () => {
        Test.assert.notEqual(sha256("a"), sha256("b"))
    })

    Test.it("hash is 64 hex characters", () => {
        Test.assert.equal(sha256("test").length, 64)
    })

    Test.it("hash only contains hex characters", () => {
        Test.assert.truthy(/^[0-9a-f]+$/.test(sha256("test")))
    })

})

Test.describe("Utils — base64 / hex", () => {

    Test.it("base64UrlToHex converts correctly", () => {
        // "hello" in base64url is "aGVsbG8"
        const hex = base64UrlToHex("aGVsbG8")
        Test.assert.equal(hex, "68656c6c6f")
    })

    Test.it("hexToBase64Url round-trips with base64UrlToHex", () => {
        const original = "68656c6c6f"
        const b64 = hexToBase64Url(original)
        const back = base64UrlToHex(b64)
        Test.assert.equal(back, original)
    })

})

Test.describe("Utils — formatNumber", () => {

    Test.it("formats integer with thousands separator for small numbers", () => {
        Test.assert.equal(formatNumber(1234), "1,234.00")
    })

    Test.it("returns only integer for numbers > 10000", () => {
        Test.assert.equal(formatNumber(1234567), "1,234,567")
    })

    Test.it("respects decimals parameter", () => {
        Test.assert.equal(formatNumber(1.5, 4), "1.5")
    })

    Test.it("strips trailing zeros from fractional part", () => {
        Test.assert.equal(formatNumber(1.5, 4), "1.5")
        Test.assert.equal(formatNumber(1.56, 4), "1.56")
    })

    Test.it("handles zero correctly", () => {
        Test.assert.equal(formatNumber(0, 2), "0.00")
    })

})

Test.describe("Utils — beautifyNumber", () => {

    Test.it("formats billions", () => {
        Test.assert.equal(beautifyNumber(2000000000), "2B")
    })

    Test.it("formats millions with decimal", () => {
        Test.assert.equal(beautifyNumber(1500000), "1.5M")
    })

    Test.it("formats millions without trailing .0", () => {
        Test.assert.equal(beautifyNumber(2000000), "2M")
    })

    Test.it("formats thousands", () => {
        Test.assert.equal(beautifyNumber(2500), "2.5K")
    })

    Test.it("returns small numbers as-is", () => {
        Test.assert.equal(beautifyNumber(999), "999")
    })

    Test.it("accepts numeric strings", () => {
        Test.assert.equal(beautifyNumber("2,000,000"), "2M")
    })

})

Test.describe("Utils — formatBalance", () => {

    Test.it("compacts thousands", () => {
        Test.assert.equal(formatBalance(1500), "1.5K")
    })

    Test.it("compacts millions", () => {
        Test.assert.equal(formatBalance(2000000), "2M")
    })

    Test.it("compacts billions", () => {
        Test.assert.equal(formatBalance(1200000000), "1.2B")
    })

    Test.it("formats small integers with no trailing decimals", () => {
        Test.assert.equal(formatBalance(42, "en"), "42")
    })

    Test.it("rounds decimals to 4 fraction digits", () => {
        Test.assert.equal(formatBalance(0.000412, "en"), "0.0004")
    })

    Test.it("strips trailing zeros from decimals", () => {
        Test.assert.equal(formatBalance(1.5, "en"), "1.5")
    })

    Test.it("handles zero", () => {
        Test.assert.equal(formatBalance(0, "en"), "0")
    })

    Test.it("returns em-dash for NaN", () => {
        Test.assert.equal(formatBalance(NaN), "—")
    })

    Test.it("returns em-dash for non-numeric string", () => {
        Test.assert.equal(formatBalance("abc"), "—")
    })

    Test.it("boundary: 999 stays as locale number", () => {
        Test.assert.equal(formatBalance(999, "en"), "999")
    })

    Test.it("boundary: 1000 switches to compact", () => {
        Test.assert.equal(formatBalance(1000), "1K")
    })

})

Test.describe("Utils — shorten", () => {

    Test.it("shortens a long string", () => {
        Test.assert.equal(shorten("abcdefghij", 3, 3), "abc...hij")
    })

    Test.it("returns string as-is when short enough", () => {
        Test.assert.equal(shorten("abc", 3, 3), "abc")
    })

    Test.it("uses default start/end of 4", () => {
        Test.assert.equal(shorten("abcdefghij"), "abcd...ghij")
    })

})

Test.describe("Utils — randomInt", () => {

    Test.it("returns integer within [min, max)", () => {
        for (let i = 0; i < 100; i++) {
            const n = randomInt(5, 10)
            Test.assert.inRange(n, 5, 9)
        }
    })

    Test.it("returns integer when no args (default 0–10000)", () => {
        const n = randomInt()
        Test.assert.inRange(n, 0, 9999)
    })

})

Test.describe("Utils — randomText", () => {

    Test.it("returns string of exact requested length", () => {
        Test.assert.equal(randomText(8).length, 8)
        Test.assert.equal(randomText(24).length, 24)
    })

    Test.it("defaults to length 24", () => {
        Test.assert.equal(randomText().length, 24)
    })

})

Test.describe("Utils — randomItem", () => {

    Test.it("returns an element from the array", () => {
        const arr = [10, 20, 30]
        for (let i = 0; i < 30; i++) 
            Test.assert.truthy(arr.includes(randomItem(arr)))
        
    })

    Test.it("returns null for non-array input", () => {
        Test.assert.equal(randomItem("not-array"), null)
    })

})

Test.describe("Utils — spintax", () => {

    Test.it("picks one of the options", () => {
        const options = ["apple", "banana", "cherry"]
        for (let i = 0; i < 30; i++) {
            const result = spintax("{apple|banana|cherry}")
            Test.assert.truthy(options.includes(result))
        }
    })

    Test.it("handles nested spintax", () => {
        const result = spintax("{a|b} and {c|d}")
        Test.assert.truthy(["a and c", "a and d", "b and c", "b and d"].includes(result))
    })

    Test.it("returns string unchanged when no spintax", () => {
        Test.assert.equal(spintax("hello"), "hello")
    })

})

Test.describe("Utils — encodeQuery", () => {

    Test.it("encodes a simple object", () => {
        const result = encodeQuery({ a: "1", b: "2" })
        Test.assert.truthy(result.includes("a=1"))
        Test.assert.truthy(result.includes("b=2"))
    })

    Test.it("encodes special characters", () => {
        const result = encodeQuery({ q: "hello world" })
        Test.assert.truthy(result.includes("hello%20world"))
    })

    Test.it("encodes object values as JSON", () => {
        const result = encodeQuery({ filter: { tag: "x" } })
        Test.assert.truthy(result.startsWith("filter="))
    })

})

Test.describe("Utils — clone", () => {

    Test.it("deep clones an object", () => {
        const obj = { a: { b: { c: 3 } } }
        const copy = clone(obj)
        Test.assert.deepEqual(copy, obj)
        Test.assert.notEqual(copy, obj) // different reference
    })

    Test.it("does not share nested references", () => {
        const obj = { arr: [1, 2, 3] }
        const copy = clone(obj)
        copy.arr.push(4)
        Test.assert.equal(obj.arr.length, 3)
    })

    Test.it("clones arrays", () => {
        const arr = [{ x: 1 }, { x: 2 }]
        const copy = clone(arr)
        Test.assert.deepEqual(copy, arr)
        Test.assert.notEqual(copy, arr)
    })

    Test.it("handles primitives", () => {
        Test.assert.equal(clone(42), 42)
        Test.assert.equal(clone("hello"), "hello")
        Test.assert.equal(clone(null), null)
    })

})

Test.describe("Utils — merge", () => {

    Test.it("merges b into a and returns a", () => {
        const a = { x: 1 }
        const b = { y: 2 }
        const result = merge(a, b)
        Test.assert.equal(result, a) // same reference
        Test.assert.equal(a.y, 2)
    })

    Test.it("overwrites existing keys from b", () => {
        const a = { x: 1, y: 1 }
        merge(a, { y: 99 })
        Test.assert.equal(a.y, 99)
    })

    Test.it("does deep merge for nested objects", () => {
        const a = { user: { name: "Alice", age: 30 } }
        merge(a, { user: { age: 31 } })
        Test.assert.equal(a.user.name, "Alice")
        Test.assert.equal(a.user.age, 31)
    })

    Test.it("returns undefined for non-objects", () => {
        Test.assert.equal(merge("a", "b"), undefined)
    })

})

Test.describe("Utils — evaluate", () => {

    Test.it("'&' returns false when any element is false", () => {
        Test.assert.equal(evaluate({ "&": [true, false, true] }), false)
    })

    Test.it("'&' returns true when all elements are true", () => {
        Test.assert.equal(evaluate({ "&": [true, true, true] }), true)
    })

    Test.it("'|' returns true when any element is true", () => {
        Test.assert.equal(evaluate({ "|": [false, true, false] }), true)
    })

    Test.it("'|' returns false when all elements are false", () => {
        Test.assert.equal(evaluate({ "|": [false, false] }), false)
    })

    Test.it("supports nested expressions", () => {
        const exp = { "&": [true, { "|": [false, true] }, { "&": [1, "x"] }] }
        Test.assert.equal(evaluate(exp), true)
    })

    Test.it("nested expression inside OR can evaluate to false", () => {
        const exp = { "|": [false, { "&": [true, 0] }] }
        Test.assert.equal(evaluate(exp), false)
    })

    Test.it("truthy scalar returns true", () => {
        Test.assert.equal(evaluate(1), true)
    })

    Test.it("falsy scalar returns false", () => {
        Test.assert.equal(evaluate(0), false)
    })

    Test.it("non-evaluate object falls back to truthiness", () => {
        Test.assert.equal(evaluate({ AND: [true, false] }), true)
    })

})
