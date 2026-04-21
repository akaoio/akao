import Test from "../Test.js"
import { launch } from "../Launcher.js"
import zen, { initZEN } from "../ZEN.js"
import { writeLabel, readLabel } from "../../UI/components/wallets/label.js"

await launch({ mode: "headless" })
await initZEN()

const PAIR = await zen.pair()

let _nextId = 0
const nextId = () => _nextId++

Test.describe("WalletLabel", () => {

    Test.it("saves and retrieves a label", async () => {
        const id = nextId()
        await writeLabel(zen, PAIR, id, "Main Wallet")
        Test.assert.equal(await readLabel(zen, PAIR.pub, id), "Main Wallet")
    })

    Test.it("overwrites a previous label", async () => {
        const id = nextId()
        await writeLabel(zen, PAIR, id, "Old")
        await writeLabel(zen, PAIR, id, "New")
        Test.assert.equal(await readLabel(zen, PAIR.pub, id), "New")
    })

    Test.it("coerces non-string to string", async () => {
        const id = nextId()
        await writeLabel(zen, PAIR, id, 42)
        Test.assert.equal(await readLabel(zen, PAIR.pub, id), "42")
    })

    Test.it("rejects when pair is unavailable", async () => {
        await Test.assert.rejects(writeLabel(zen, null, nextId(), "x"), "No zen/pair")
    })

})
