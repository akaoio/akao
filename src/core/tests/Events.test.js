import Test from "../Test.js"
import { Events } from "../Events.js"

Test.describe("Events", () => {

    Test.it("emits and receives events", () => {
        const e = new Events()
        let received = null
        e.on("test", ({ detail }) => { received = detail })
        e.emit("test", "hello")
        Test.assert.equal(received, "hello")
    })

    Test.it("wraps detail in object", () => {
        const e = new Events()
        let event = null
        e.on("evt", (ev) => { event = ev })
        e.emit("evt", 42)
        Test.assert.truthy("detail" in event, "event should have .detail")
        Test.assert.equal(event.detail, 42)
    })

    Test.it("supports multiple listeners for the same event", () => {
        const e = new Events()
        const results = []
        e.on("multi", ({ detail }) => results.push("a:" + detail))
        e.on("multi", ({ detail }) => results.push("b:" + detail))
        e.emit("multi", 1)
        Test.assert.deepEqual(results, ["a:1", "b:1"])
    })

    Test.it("unsubscribe via returned function stops notifications", () => {
        const e = new Events()
        let count = 0
        const off = e.on("unsub", () => { count++ })
        e.emit("unsub", null)
        off()
        e.emit("unsub", null)
        Test.assert.equal(count, 1)
    })

    Test.it("unsubscribe via off.off works identically", () => {
        const e = new Events()
        let count = 0
        const off = e.on("unsub2", () => { count++ })
        e.emit("unsub2", null)
        off.off()
        e.emit("unsub2", null)
        Test.assert.equal(count, 1)
    })

    Test.it("separate instances are independent", () => {
        const a = new Events()
        const b = new Events()
        let aCount = 0
        let bCount = 0
        a.on("ping", () => aCount++)
        b.on("ping", () => bCount++)
        a.emit("ping", null)
        Test.assert.equal(aCount, 1)
        Test.assert.equal(bCount, 0)
    })

    Test.it("emitting with no listeners does not throw", () => {
        const e = new Events()
        e.emit("noop", "value")
    })

    Test.it("detail can be an object", () => {
        const e = new Events()
        let received = null
        e.on("obj", ({ detail }) => { received = detail })
        e.emit("obj", { x: 1, y: 2 })
        Test.assert.deepEqual(received, { x: 1, y: 2 })
    })

    Test.it("once triggers listener only once", () => {
        const e = new Events()
        let count = 0
        e.once("once", () => { count++ })
        e.emit("once", null)
        e.emit("once", null)
        Test.assert.equal(count, 1)
    })

    Test.it("emit supports browser event options", () => {
        const e = new Events()
        const ev = e.emit("options", { ok: true }, {
            bubbles: true,
            composed: true,
            cancelable: true
        })
        Test.assert.equal(ev.bubbles, true)
        Test.assert.equal(ev.composed, true)
        Test.assert.equal(ev.cancelable, true)
        Test.assert.deepEqual(ev.detail, { ok: true })
    })

    Test.it("can use external EventTarget as dispatch target", () => {
        const target = new EventTarget()
        const e = new Events(target)
        let received = null
        target.addEventListener("target", ({ detail }) => { received = detail })
        e.emit("target", "ok")
        Test.assert.equal(received, "ok")
    })

})
