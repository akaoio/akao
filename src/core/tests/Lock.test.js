import Test from "../Test.js"
import { sha256 } from "../Utils/crypto.js"
import { Lock } from "../Lock.js"
import { devplatform } from "../Platform.js"
import { HDNodeWallet, getBytes } from "../Ethers.js"

const _SEA = globalThis.sea

const PLATFORM = await devplatform({ sea: _SEA })
const [PAYER_PAIR] = await Promise.all([
    _SEA.pair()
])

const RECIPIENT_ROOT = HDNodeWallet.fromSeed(getBytes("0x" + sha256("lock-recipient-root")))
const RECIPIENT_XPUB = RECIPIENT_ROOT.neuter().extendedKey

function makeLock(type = "TL") {
    return new Lock({
        payer: PAYER_PAIR,
        platform: PLATFORM.pair,
        recipient: { xpub: RECIPIENT_XPUB },
        tradeId: "trade-lock-1",
        type
    })
}

Test.describe("Lock — platform derivation primitives", () => {

    Test.it("constructor throws invalidInput when tradeId is missing", () => {
        Test.assert.throws(() => new Lock({
            payer: PAYER_PAIR,
            platform: PLATFORM.pair,
            recipient: { xpub: RECIPIENT_XPUB }
        }), "invalidInput")
    })

    Test.it("secret() matches SEA shared secret with platform epub", async () => {
        const lock = makeLock()
        const expected = await _SEA.secret(PLATFORM.epub, PAYER_PAIR)
        Test.assert.equal(await lock.secret(), expected)
    })

    Test.it("index() is deterministic for the same trade + type", async () => {
        const a = makeLock("TL")
        const b = makeLock("TL")
        Test.assert.equal(await a.index(), await b.index())
    })

    Test.it("TL and CL derive different indexes for the same trade", async () => {
        const tl = makeLock("TL")
        const cl = makeLock("CL")
        Test.assert.notEqual(await tl.index(), await cl.index())
    })

    Test.it("address() and unlock() resolve the same child wallet", async () => {
        const lock = makeLock("TL")
        const address = await lock.address()
        const unlocked = await lock.unlock(RECIPIENT_ROOT.extendedKey)
        Test.assert.equal(unlocked.address, address)
    })

    Test.it("address() rejects when recipient xpub is missing", async () => {
        const lock = new Lock({
            payer: PAYER_PAIR,
            platform: PLATFORM.pair,
            recipient: {},
            tradeId: "trade-lock-2",
            type: "TL"
        })
        await Test.assert.rejects(lock.address(), "xpubRequired")
    })

    Test.it("unlock() rejects when xprv is missing", async () => {
        const lock = makeLock("TL")
        await Test.assert.rejects(lock.unlock(), "xprvRequired")
    })

    Test.it("index() rejects unknown lock types", async () => {
        const lock = makeLock("XX")
        await Test.assert.rejects(lock.index(), "invalidLockType")
    })

})
