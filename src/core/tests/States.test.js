import Test from "../Test.js"
import States from "../States.js"

Test.describe("States", () => {

    Test.it("initialises with provided state", () => {
        const s = new States({ x: 1, y: "hello" })
        Test.assert.equal(s.get("x"), 1)
        Test.assert.equal(s.get("y"), "hello")
    })

    Test.it("get returns undefined for unknown key", () => {
        const s = new States()
        Test.assert.equal(s.get("missing"), undefined)
    })

    Test.it("set adds a new key-value pair", () => {
        const s = new States()
        s.set({ count: 5 })
        Test.assert.equal(s.get("count"), 5)
    })

    Test.it("set with string sets that key to true", () => {
        const s = new States()
        s.set("flag")
        Test.assert.equal(s.get("flag"), true)
    })

    Test.it("set with array sets each key to true", () => {
        const s = new States()
        s.set(["a", "b", "c"])
        Test.assert.equal(s.get("a"), true)
        Test.assert.equal(s.get("b"), true)
        Test.assert.equal(s.get("c"), true)
    })

    Test.it("notifications fire only when value actually changes", () => {
        const s = new States({ n: 10 })
        let calls = 0
        s.on("n", () => calls++)
        s.set({ n: 10 }) // same value — no notification
        Test.assert.equal(calls, 0)
        s.set({ n: 11 })
        Test.assert.equal(calls, 1)
    })

    Test.it("multiple subscribers for the same key all receive the update", () => {
        const s = new States()
        const received = []
        s.on("v", ({ value }) => received.push("a:" + value))
        s.on("v", ({ value }) => received.push("b:" + value))
        s.set({ v: 42 })
        Test.assert.deepEqual(received, ["a:42", "b:42"])
    })

    Test.it("on() returns an unsubscribe function", () => {
        const s = new States()
        let count = 0
        const off = s.on("k", () => count++)
        s.set({ k: 1 })
        off()
        s.set({ k: 2 })
        Test.assert.equal(count, 1)
    })

    Test.it("global SET subscriber fires on any state change", () => {
        const s = new States()
        const keys = []
        s.on(({ key }) => keys.push(key))
        s.set({ a: 1 })
        s.set({ b: 2 })
        Test.assert.deepEqual(keys, ["a", "b"])
    })

    Test.it("global subscriber unsubscribes correctly", () => {
        const s = new States()
        let count = 0
        const off = s.on(() => count++)
        s.set({ x: 1 })
        off()
        s.set({ x: 2 })
        Test.assert.equal(count, 1)
    })

    Test.it("has() returns true for existing keys", () => {
        const s = new States({ a: 1 })
        Test.assert.truthy(s.has("a"))
        Test.assert.falsy(s.has("b"))
    })

    Test.it("has() checks multiple keys (array)", () => {
        const s = new States({ a: 1, b: 2 })
        Test.assert.truthy(s.has(["a", "b"]))
        Test.assert.falsy(s.has(["a", "c"]))
    })

    Test.it("del() removes a key", () => {
        const s = new States({ d: 99 })
        s.del("d")
        Test.assert.falsy(s.has("d"))
        Test.assert.equal(s.get("d"), undefined)
    })

    Test.it("clear() clears subscribers for a key", () => {
        const s = new States({ a: 1 })
        let calls = 0
        s.on("a", () => calls++)
        s.clear("a")  // clears subscribers
        s.set({ a: 2 })  // should not notify cleared subscriber
        Test.assert.equal(calls, 0)
        Test.assert.equal(s.get("a"), 2)  // state still has value
    })

    Test.it("same() performs deep equality check", () => {
        const s = new States()
        Test.assert.truthy(s.same({ x: 1 }, { x: 1 }))
        Test.assert.falsy(s.same({ x: 1 }, { x: 2 }))
        Test.assert.truthy(s.same([1, 2], [1, 2]))
        Test.assert.falsy(s.same([1, 2], [1, 3]))
        Test.assert.truthy(s.same(null, null))
        Test.assert.falsy(s.same(null, undefined))
    })

    Test.it("get() with array path traverses nested object", () => {
        const s = new States({ user: { name: "Alice" } })
        Test.assert.equal(s.get(["user", "name"]), "Alice")
    })

    Test.it("MAP subscriber with array key fires on nested path change", () => {
        const s = new States({ obj: { inner: 0 } })
        let received = null
        s.on(["obj", "inner"], ({ value }) => { received = value })
        s.set({ obj: { inner: 42 } })
        Test.assert.equal(received, 42)
    })

    Test.it("on() with immediate=true calls subscriber with current value", () => {
        const s = new States({ now: 7 })
        let received = null
        s.on("now", ({ value }) => { received = value }, true)
        Test.assert.equal(received, 7)
    })

    Test.it("deep equality prevents notification for identical objects", () => {
        const s = new States({ obj: { a: 1 } })
        let count = 0
        s.on("obj", () => count++)
        s.set({ obj: { a: 1 } }) // structurally same
        Test.assert.equal(count, 0)
    })

})
