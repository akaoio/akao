import Test from "../Test.js"
import { launch } from "../Launcher.js"
import { writeLabel, readLabel } from "../../UI/components/wallets/label.js"

await launch({ mode: "headless" })

const PAIR = await globalThis.sea.pair()

let _nextId = 0
const nextId = () => _nextId++

Test.describe("WalletLabel", () => {

    Test.it("saves and retrieves a label", async () => {
        const id = nextId()
        await writeLabel(PAIR, id, "Main Wallet")
        Test.assert.equal(await readLabel(PAIR.pub, id), "Main Wallet")
    })

    Test.it("overwrites a previous label", async () => {
        const id = nextId()
        await writeLabel(PAIR, id, "Old")
        await writeLabel(PAIR, id, "New")
        Test.assert.equal(await readLabel(PAIR.pub, id), "New")
    })

    Test.it("coerces non-string to string", async () => {
        const id = nextId()
        await writeLabel(PAIR, id, 42)
        Test.assert.equal(await readLabel(PAIR.pub, id), "42")
    })

    Test.it("rejects when pair is unavailable", async () => {
        await Test.assert.rejects(writeLabel(null, nextId(), "x"), "No gun/pair")
    })

})
