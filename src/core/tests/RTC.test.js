import Test from "../Test.js"
import RTC from "../RTC.js"

Test.describe("RTC", () => {

    Test.it("registers and unregisters event listeners", () => {
        const rtc = new RTC()
        let hits = 0
        const off = rtc.on("state", () => { hits++ })

        rtc.$emit("state", rtc.snapshot())
        Test.assert.equal(hits, 1)

        off()
        rtc.$emit("state", rtc.snapshot())
        Test.assert.equal(hits, 1)
    })

    Test.it("snapshot exposes disconnected defaults", () => {
        const rtc = new RTC({ channel: "mesh" })
        const state = rtc.snapshot()

        Test.assert.equal(state.channel, "mesh")
        Test.assert.equal(state.connectionState, "idle")
        Test.assert.equal(state.dataState, "closed")
        Test.assert.deepEqual(state.candidates, [])
    })

    Test.it("send fails before the data channel opens", () => {
        const rtc = new RTC()
        Test.assert.throws(() => rtc.send("hello"), "not open")
    })

})