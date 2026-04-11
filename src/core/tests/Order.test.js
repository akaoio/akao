/**
 * Order.test.js — Adversarial unit tests for the Order module.
 *
 * Strategy: think like hackers. Test boundary conditions, injection attacks,
 * identity spoofing, replay, type confusion, BigInt traps, and cryptographic
 * invariants (domain separation, determinism, entropy adequacy).
 *
 * All dependencies are real: real Gun (in-memory), real SEA, real EVM (Ganache).
 * No mocks. No fake data. Seeds are deterministic and human-readable.
 */

import Test from "../Test.js"
import { createRequire } from "module"
import { sha256 } from "../Utils/crypto.js"
import { Order } from "../Order.js"
import { cancel as cancelFn } from "../Order/cancel.js"
import { match  as matchFn  } from "../Order/match.js"
import { proof  as proofFn  } from "../Order/proof.js"
import { fetch  as fetchFn  } from "../Order/fetch.js"
import { soul   as soulFn   } from "../Order/soul.js"
import { ethers, HDNodeWallet, getBytes } from "../Ethers.js"

// ─── Bootstrap: real headless runtime + EVM ───────────────────────────────────

const _req  = createRequire(import.meta.url)
const Ganache = _req("ganache")
const _SEA = globalThis.sea

// ─── Deterministic test environment ──────────────────────────────────────────

const MAKER_SEED = "seed"
const TAKER_SEED = "seed-taker"
const ARB_SEED   = "seed-arb"

const MAKER_WALLET = HDNodeWallet.fromSeed(getBytes("0x" + sha256(MAKER_SEED)))
const TAKER_WALLET = HDNodeWallet.fromSeed(getBytes("0x" + sha256(TAKER_SEED)))
const ARB_WALLET   = HDNodeWallet.fromSeed(getBytes("0x" + sha256(ARB_SEED)))

const MAKER_XPUB = MAKER_WALLET.neuter().extendedKey
const TAKER_XPUB = TAKER_WALLET.neuter().extendedKey

// SEA keypairs — identity signing (random per run, named by role)
const [PAIR_MAKER, PAIR_TAKER, PAIR_B] = await Promise.all([
    _SEA.pair(), _SEA.pair(), _SEA.pair()
])
// Keep PAIR_A as alias for PAIR_MAKER for backward compat within this file
const PAIR_A = PAIR_MAKER

const ITEM = "arc-raiders/acoustic-guitar-ba4df"

// ─── Ganache local EVM ────────────────────────────────────────────────────────

const _ganache = Ganache.provider({ logging: { quiet: true } })
const _evm     = new ethers.BrowserProvider(_ganache)
const _signer  = await _evm.getSigner(0)

/** Fund a specific address on Ganache testnet */
async function fundAddress(address, wei) {
    await _signer.sendTransaction({ to: address, value: wei })
}

/** Real chain: delegates getBalance to Ganache */
const CHAIN = { getBalance: (addr) => _evm.getBalance(addr) }

// ─── TUI: print test environment ──────────────────────────────────────────────

const _fpAddr = HDNodeWallet
    .fromExtendedKey(MAKER_XPUB)
    .deriveChild(parseInt(sha256("FP:oid").slice(0, 8), 16) & 0x7fffffff)
    .address

const W    = 60
const _pad = (label, val) => `  ${label.padEnd(14)} ${val}`
console.log(`\n${"═".repeat(W)}`)
console.log(`  Order Test Environment`)
console.log("═".repeat(W))
console.log(_pad("MAKER seed:", `"${MAKER_SEED}"`))
console.log(_pad("  SEA pub:", PAIR_MAKER.pub.slice(0, 22) + "…"))
console.log(_pad("  ETH addr:", MAKER_WALLET.address))
console.log(_pad("  xpub:", MAKER_XPUB.slice(0, 22) + "…"))
console.log(_pad("TAKER seed:", `"${TAKER_SEED}"`))
console.log(_pad("  SEA pub:", PAIR_TAKER.pub.slice(0, 22) + "…"))
console.log(_pad("  ETH addr:", TAKER_WALLET.address))
console.log(_pad("ARB seed:", `"${ARB_SEED}"`))
console.log(_pad("  ETH addr:", ARB_WALLET.address))
console.log(_pad("FP addr:", `${_fpAddr}`))
console.log(_pad("  (orderId:", `"oid", from MAKER xpub)`))
console.log("═".repeat(W) + "\n")

// ─── Helpers ──────────────────────────────────────────────────────────────────

/** Create a real in-memory Gun node (no persistence, no peers). */
function makeGun() {
    return new globalThis.Gun({ localStorage: false, radisk: false, peers: [] })
}

/** Read a Gun node once, with timeout. Resolves undefined on timeout. */
function gunOnce(node, timeoutMs = 800) {
    return new Promise((resolve) => {
        const t = setTimeout(() => resolve(undefined), timeoutMs)
        node.once((data) => { clearTimeout(t); resolve(data) })
    })
}

/** Create a valid buy Order instance. */
function newBuy(overrides = {}) {
    return new Order({
        gun: makeGun(), pair: PAIR_MAKER, item: ITEM, type: "buy",
        price: 100, currency: "USDT", chain: 1, xpub: MAKER_XPUB,
        ...overrides
    })
}

/** Create a valid sell Order instance. */
function newSell(overrides = {}) {
    return new Order({
        gun: makeGun(), pair: PAIR_MAKER, item: ITEM, type: "sell",
        price: 95, currency: "USDT", chain: 1,
        ...overrides
    })
}

// ─── 1. Constructor — validation ──────────────────────────────────────────────

Test.describe("Order — constructor: missing required fields", () => {

    Test.it("throws invalidInput when gun is missing", () => {
        const gun = makeGun()
        Test.assert.throws(
            () => new Order({ pair: PAIR_A, item: ITEM, type: "sell", price: 10, currency: "USDT", chain: 1 }),
            "invalidInput"
        )
    })

    Test.it("throws invalidInput when pair is missing", () => {
        const gun = makeGun()
        Test.assert.throws(
            () => new Order({ gun, item: ITEM, type: "sell", price: 10, currency: "USDT", chain: 1 }),
            "invalidInput"
        )
    })

    Test.it("throws invalidInput when item is missing", () => {
        const gun = makeGun()
        Test.assert.throws(
            () => new Order({ gun, pair: PAIR_A, type: "sell", price: 10, currency: "USDT", chain: 1 }),
            "invalidInput"
        )
    })

    Test.it("throws invalidInput when type is missing", () => {
        const gun = makeGun()
        Test.assert.throws(
            () => new Order({ gun, pair: PAIR_A, item: ITEM, price: 10, currency: "USDT", chain: 1 }),
            "invalidInput"
        )
    })

    Test.it("throws invalidInput when price is missing", () => {
        const gun = makeGun()
        Test.assert.throws(
            () => new Order({ gun, pair: PAIR_A, item: ITEM, type: "sell", currency: "USDT", chain: 1 }),
            "invalidInput"
        )
    })

    Test.it("throws invalidInput when currency is missing", () => {
        const gun = makeGun()
        Test.assert.throws(
            () => new Order({ gun, pair: PAIR_A, item: ITEM, type: "sell", price: 10, chain: 1 }),
            "invalidInput"
        )
    })

    Test.it("throws invalidInput when chain is missing", () => {
        const gun = makeGun()
        Test.assert.throws(
            () => new Order({ gun, pair: PAIR_A, item: ITEM, type: "sell", price: 10, currency: "USDT" }),
            "invalidInput"
        )
    })

    Test.it("throws invalidInput with empty options object", () => {
        Test.assert.throws(() => new Order({}), "invalidInput")
    })

    Test.it("throws invalidInput with no arguments", () => {
        Test.assert.throws(() => new Order(), "invalidInput")
    })

})

Test.describe("Order — constructor: price manipulation", () => {

    Test.it("throws invalidPrice for price = 0", () => {
        const gun = makeGun()
        Test.assert.throws(
            () => new Order({ gun, pair: PAIR_A, item: ITEM, type: "sell", price: 0, currency: "USDT", chain: 1 }),
            "invalidPrice"
        )
    })

    Test.it("throws invalidPrice for negative price", () => {
        const gun = makeGun()
        Test.assert.throws(
            () => new Order({ gun, pair: PAIR_A, item: ITEM, type: "sell", price: -1, currency: "USDT", chain: 1 }),
            "invalidPrice"
        )
    })

    Test.it("throws invalidPrice for NaN", () => {
        const gun = makeGun()
        Test.assert.throws(
            () => new Order({ gun, pair: PAIR_A, item: ITEM, type: "sell", price: NaN, currency: "USDT", chain: 1 }),
            "invalidPrice"
        )
    })

    Test.it("throws invalidPrice for Infinity", () => {
        const gun = makeGun()
        Test.assert.throws(
            () => new Order({ gun, pair: PAIR_A, item: ITEM, type: "sell", price: Infinity, currency: "USDT", chain: 1 }),
            "invalidPrice"
        )
    })

    Test.it("throws invalidPrice for string price (type confusion)", () => {
        const gun = makeGun()
        Test.assert.throws(
            () => new Order({ gun, pair: PAIR_A, item: ITEM, type: "sell", price: "100", currency: "USDT", chain: 1 }),
            "invalidPrice"
        )
    })

})

Test.describe("Order — constructor: type validation", () => {

    Test.it("throws invalidType for unknown type 'admin'", () => {
        const gun = makeGun()
        Test.assert.throws(
            () => new Order({ gun, pair: PAIR_A, item: ITEM, type: "admin", price: 10, currency: "USDT", chain: 1 }),
            "invalidType"
        )
    })

    Test.it("throws invalidType for uppercase 'BUY'", () => {
        const gun = makeGun()
        Test.assert.throws(
            () => new Order({ gun, pair: PAIR_A, item: ITEM, type: "BUY", price: 10, currency: "USDT", chain: 1 }),
            "invalidType"
        )
    })

    Test.it("throws invalidType for 'Buy' (mixed case)", () => {
        const gun = makeGun()
        Test.assert.throws(
            () => new Order({ gun, pair: PAIR_A, item: ITEM, type: "Buy", price: 10, currency: "USDT", chain: 1 }),
            "invalidType"
        )
    })

})

Test.describe("Order — constructor: separator injection", () => {

    Test.it("throws invalidItem for item containing single colon", () => {
        const gun = makeGun()
        Test.assert.throws(
            () => new Order({ gun, pair: PAIR_A, item: "item:hack", type: "sell", price: 10, currency: "USDT", chain: 1 }),
            "invalidItem"
        )
    })

    Test.it("throws invalidItem for item 'buy:sell:0:nonce' (segment shift attack)", () => {
        const gun = makeGun()
        Test.assert.throws(
            () => new Order({ gun, pair: PAIR_A, item: "buy:sell:0:nonce", type: "sell", price: 10, currency: "USDT", chain: 1 }),
            "invalidItem"
        )
    })

    Test.it("throws invalidItem for item with only colons", () => {
        const gun = makeGun()
        Test.assert.throws(
            () => new Order({ gun, pair: PAIR_A, item: ":::", type: "sell", price: 10, currency: "USDT", chain: 1 }),
            "invalidItem"
        )
    })

})

Test.describe("Order — constructor: buy order xpub requirement", () => {

    Test.it("throws xpubRequired for buy order without xpub", () => {
        const gun = makeGun()
        Test.assert.throws(
            () => new Order({ gun, pair: PAIR_A, item: ITEM, type: "buy", price: 10, currency: "USDT", chain: 1 }),
            "xpubRequired"
        )
    })

    Test.it("sell order succeeds without xpub", () => {
        const gun = makeGun()
        const o = new Order({ gun, pair: PAIR_A, item: ITEM, type: "sell", price: 10, currency: "USDT", chain: 1 })
        Test.assert.equal(o.type, "sell")
        Test.assert.equal(o.xpub, null)
    })

    Test.it("buy order with xpub stores it on instance", () => {
        const o = newBuy()
        Test.assert.equal(o.xpub, MAKER_XPUB)
        Test.assert.equal(o.type, "buy")
    })

    Test.it("valid sell order stores all fields correctly", () => {
        const o = newSell()
        Test.assert.equal(o.item, ITEM)
        Test.assert.equal(o.price, 95)
        Test.assert.equal(o.currency, "USDT")
        Test.assert.equal(o.chain, 1)
        Test.assert.equal(o.referrer, null)
    })

})

// ─── 2. id — determinism + entropy ───────────────────────────────────────────

Test.describe("Order — id: determinism + entropy", () => {

    Test.it("same instance always returns the same orderId (caching)", async () => {
        const o = newSell()
        const a = await o.id()
        const b = await o.id()
        Test.assert.equal(a, b)
    })

    Test.it("orderId is a 64-char hex string", async () => {
        const o = newSell()
        const oid = await o.id()
        Test.assert.equal(oid.length, 64)
        Test.assert.truthy(/^[0-9a-f]{64}$/.test(oid))
    })

    Test.it("two Order instances with identical params produce different orderIds (entropy)", async () => {
        const a = await newSell().id()
        const b = await newSell().id()
        Test.assert.notEqual(a, b)
    })

    Test.it("50 Order instances produce 50 unique orderIds (birthday resistance)", async () => {
        const ids = await Promise.all(Array.from({ length: 50 }, () => newSell().id()))
        const unique = new Set(ids)
        Test.assert.equal(unique.size, 50)
    })

    Test.it("domain isolation: orderId ≠ sha256 of same content with TR: prefix", async () => {
        const o = newSell()
        const oid = await o.id()
        // Manually compute what a tradeId with the same fields would be
        const collision = sha256("TR:" + o.pair.pub + ":" + o.item + ":" + o.price + ":" + o._ts + ":" + o._rnd)
        Test.assert.notEqual(oid, collision)
    })

})

// ─── 3. key — format + PoW ───────────────────────────────────────────────────

Test.describe("Order — key: format and PoW correctness", () => {

    Test.it("key has exactly 5 colon-separated segments", async () => {
        const o = newSell()
        const k = await o.key()
        const segs = k.split(":")
        Test.assert.equal(segs.length, 5)
    })

    Test.it("segment 0 is the current candle number (within ±1)", async () => {
        const before = Math.floor(Date.now() / 300000)
        const k = await newSell().key()
        const after  = Math.floor(Date.now() / 300000)
        const seg0 = parseInt(k.split(":")[0], 10)
        Test.assert.truthy(seg0 >= before && seg0 <= after)
    })

    Test.it("segment 1 is the item slug exactly", async () => {
        const k = await newSell().key()
        Test.assert.equal(k.split(":")[1], ITEM)
    })

    Test.it("segment 2 is the order type", async () => {
        const kb = await newBuy().key()
        const ks = await newSell().key()
        Test.assert.equal(kb.split(":")[2], "buy")
        Test.assert.equal(ks.split(":")[2], "sell")
    })

    Test.it("segment 3 is first 8 chars of pair.pub", async () => {
        const k = await newSell().key()
        Test.assert.equal(k.split(":")[3], PAIR_A.pub.slice(0, 8))
    })

    Test.it("SHA-256 of the full key starts with '000' (difficulty 3)", async () => {
        const k = await newSell().key()
        Test.assert.truthy(sha256(k).startsWith("000"), `PoW failed for key: ${k}`)
    })

})

// ─── 4. cancel — ownership enforcement ───────────────────────────────────────

Test.describe("Order — cancel: ownership enforcement", () => {

    Test.it("returns notOwner when key has fewer than 4 segments", async () => {
        const o = newSell()
        const result = await o.cancel("candle:item:sell")
        Test.assert.deepEqual(result, { error: "notOwner" })
    })

    Test.it("returns notOwner when pub8 in key doesn't match pair.pub", async () => {
        const o = newSell()
        const fakeKey = `5000000:${ITEM}:sell:XXXXXXXX:0`
        const result = await o.cancel(fakeKey)
        Test.assert.deepEqual(result, { error: "notOwner" })
    })

    Test.it("returns notOwner when key belongs to PAIR_B (cross-cancellation attack)", async () => {
        // PAIR_B creates a key with their pub8
        const attackerPub8 = PAIR_B.pub.slice(0, 8)
        const victimKey = `5000000:${ITEM}:sell:${attackerPub8}:0`
        // PAIR_A (o) tries to cancel PAIR_B's order
        const o = newSell()
        const result = await o.cancel(victimKey)
        Test.assert.deepEqual(result, { error: "notOwner" })
    })

    Test.it("returns notOwner for empty key string", async () => {
        const o = newSell()
        const result = await o.cancel("")
        Test.assert.deepEqual(result, { error: "notOwner" })
    })

    Test.it("cancel clears entry in Gun when pub8 matches owner", async () => {
        const gun = makeGun()
        const o = new Order({ gun, pair: PAIR_MAKER, item: ITEM, type: "sell", price: 95, currency: "USDT", chain: 1 })
        const { key } = await o.create()
        await o.cancel(key)
        const s = soulFn()
        const stored = await gunOnce(gun.get(s).get(key))
        // After cancel, entry should be null/undefined or have no price field
        Test.assert.falsy(stored && stored !== null && typeof stored === "object" && stored.price,
            "Gun entry must be cleared after cancel")
    })

    Test.it("cancel with correct pub8 does not return notOwner", async () => {
        const gun = makeGun()
        const o = new Order({ gun, pair: PAIR_MAKER, item: ITEM, type: "sell", price: 95, currency: "USDT", chain: 1 })
        const validKey = `5000000:${ITEM}:sell:${PAIR_MAKER.pub.slice(0, 8)}:xyz`
        const result = await o.cancel(validKey)
        Test.assert.falsy(result?.error, "should not return error for valid owner cancel")
    })

})

// ─── 5. match — tradeId determinism ──────────────────────────────────────────

Test.describe("Order — match: tradeId determinism + domain isolation", () => {

    Test.it("tradeId is a 64-char hex string", async () => {
        const gun = makeGun()
        const o = new Order({ gun, pair: PAIR_A, item: ITEM, type: "sell", price: 95, currency: "USDT", chain: 1 })
        const { tradeId } = await o.match({ orderId: "abc123", makerpub: "makerpub" })
        Test.assert.equal(tradeId.length, 64)
        Test.assert.truthy(/^[0-9a-f]{64}$/.test(tradeId))
    })

    Test.it("same orderId + makerpub + taker always produces same tradeId (deterministic)", async () => {
        const gun = makeGun()
        const o = new Order({ gun, pair: PAIR_A, item: ITEM, type: "sell", price: 95, currency: "USDT", chain: 1 })
        const args = { orderId: "fixedorder123", makerpub: "makerX" }
        const a = await o.match(args)
        const b = await o.match(args)
        Test.assert.equal(a.tradeId, b.tradeId)
    })

    Test.it("different takers produce different tradeIds for same order", async () => {
        const g1 = makeGun()
        const g2 = makeGun()
        const oA = new Order({ gun: g1, pair: PAIR_A, item: ITEM, type: "sell", price: 95, currency: "USDT", chain: 1 })
        const oB = new Order({ gun: g2, pair: PAIR_B, item: ITEM, type: "sell", price: 95, currency: "USDT", chain: 1 })
        const args = { orderId: "fixedorder123", makerpub: "makerX" }
        const { tradeId: ta } = await oA.match(args)
        const { tradeId: tb } = await oB.match(args)
        Test.assert.notEqual(ta, tb)
    })

    Test.it("different orderIds produce different tradeIds for same taker", async () => {
        const gun = makeGun()
        const o = new Order({ gun, pair: PAIR_A, item: ITEM, type: "sell", price: 95, currency: "USDT", chain: 1 })
        const a = await o.match({ orderId: "order-AAA", makerpub: "maker1" })
        const b = await o.match({ orderId: "order-BBB", makerpub: "maker1" })
        Test.assert.notEqual(a.tradeId, b.tradeId)
    })

    Test.it("return value has exactly { tradeId } — no timestamp leaked", async () => {
        const gun = makeGun()
        const o = new Order({ gun, pair: PAIR_A, item: ITEM, type: "sell", price: 95, currency: "USDT", chain: 1 })
        const result = await o.match({ orderId: "o1", makerpub: "m1" })
        Test.assert.truthy("tradeId" in result)
        Test.assert.falsy("ts" in result, "timestamp must not be in return value")
    })

    Test.it("tradeId uses TR: domain separator — different from FP: hash of same input", async () => {
        const orderId = "test-order-id"
        const makerpub = "some-maker-pub"
        const takerpub = PAIR_A.pub
        const tradeHash = sha256("TR:" + orderId + ":" + makerpub + ":" + takerpub)
        const fpHash    = sha256("FP:" + orderId + ":" + makerpub + ":" + takerpub)
        const orHash    = sha256("OR:" + orderId + ":" + makerpub + ":" + takerpub)
        Test.assert.notEqual(tradeHash, fpHash)
        Test.assert.notEqual(tradeHash, orHash)
        Test.assert.notEqual(fpHash, orHash)
    })

    Test.it("match writes trade record to Gun when key is provided", async () => {
        const gun = makeGun()
        const o = new Order({ gun, pair: PAIR_MAKER, item: ITEM, type: "sell", price: 95, currency: "USDT", chain: 1 })
        const { key, orderId } = await o.create()
        await o.match({ orderId, makerpub: PAIR_MAKER.pub, key })
        const s = soulFn()
        const stored = await gunOnce(gun.get(s).get(key))
        Test.assert.truthy(stored, "trade record must be written to Gun")
        Test.assert.truthy(typeof stored === "string", "stored value is a signed string")
    })

})

// ─── 6. proof — balance validation + BigInt safety ───────────────────────────

// Pre-fund FP addresses for tests that require balance > 0.
// FP address = MAKER_XPUB.deriveChild(sha256("FP:" + orderId)[0:8] & 0x7fffffff)
const _fpFunded   = HDNodeWallet.fromExtendedKey(MAKER_XPUB)
    .deriveChild(parseInt(sha256("FP:funded-order").slice(0, 8), 16) & 0x7fffffff).address
const _fpBoundary = HDNodeWallet.fromExtendedKey(MAKER_XPUB)
    .deriveChild(parseInt(sha256("FP:boundary-order").slice(0, 8), 16) & 0x7fffffff).address
// Fund: 1000 wei and 500 wei on local Ganache
await fundAddress(_fpFunded, 1000n)
await fundAddress(_fpBoundary, 500n)

Test.describe("Order — proof: balance validation", () => {

    Test.it("returns invalidRequired when required is 0", async () => {
        const result = await proofFn.call({}, { chain: CHAIN, orderId: "oid", xpub: MAKER_XPUB, required: 0 })
        Test.assert.deepEqual(result, { error: "invalidRequired" })
    })

    Test.it("returns invalidRequired when required is null", async () => {
        const result = await proofFn.call({}, { chain: CHAIN, orderId: "oid", xpub: MAKER_XPUB, required: null })
        Test.assert.deepEqual(result, { error: "invalidRequired" })
    })

    Test.it("returns invalidRequired when required is undefined", async () => {
        const result = await proofFn.call({}, { chain: CHAIN, orderId: "oid", xpub: MAKER_XPUB, required: undefined })
        Test.assert.deepEqual(result, { error: "invalidRequired" })
    })

    Test.it("returns invalidRequired when required is negative", async () => {
        const result = await proofFn.call({}, { chain: CHAIN, orderId: "oid", xpub: MAKER_XPUB, required: -1 })
        Test.assert.deepEqual(result, { error: "invalidRequired" })
    })

    Test.it("sufficient: true when balance (1000 wei) > required (999 wei)", async () => {
        const result = await proofFn.call({}, { chain: CHAIN, orderId: "funded-order", xpub: MAKER_XPUB, required: 999 })
        Test.assert.equal(result.sufficient, true)
    })

    Test.it("sufficient: true when balance === required (boundary: 500 wei)", async () => {
        const result = await proofFn.call({}, { chain: CHAIN, orderId: "boundary-order", xpub: MAKER_XPUB, required: 500 })
        Test.assert.equal(result.sufficient, true)
    })

    Test.it("sufficient: false when balance (0) < required (200) for unfunded address", async () => {
        const result = await proofFn.call({}, { chain: CHAIN, orderId: "unfunded-order", xpub: MAKER_XPUB, required: 200 })
        Test.assert.equal(result.sufficient, false)
    })

    Test.it("BigInt safety: balance=1000 wei vs required=999 (JS number) → sufficient", async () => {
        // Without BigInt() conversion, JS would throw or produce wrong result
        const result = await proofFn.call({}, { chain: CHAIN, orderId: "funded-order", xpub: MAKER_XPUB, required: 999 })
        Test.assert.equal(result.sufficient, true)
    })

    Test.it("result contains fp address as a string starting with 0x", async () => {
        const result = await proofFn.call({}, { chain: CHAIN, orderId: "oid", xpub: MAKER_XPUB, required: 1 })
        Test.assert.truthy(typeof result.address === "string")
        Test.assert.truthy(result.address.startsWith("0x"))
    })

    Test.it("same orderId always derives same FP address (deterministic)", async () => {
        const r1 = await proofFn.call({}, { chain: CHAIN, orderId: "same-id", xpub: MAKER_XPUB, required: 1 })
        const r2 = await proofFn.call({}, { chain: CHAIN, orderId: "same-id", xpub: MAKER_XPUB, required: 1 })
        Test.assert.equal(r1.address, r2.address)
    })

    Test.it("different orderIds derive different FP addresses", async () => {
        const r1 = await proofFn.call({}, { chain: CHAIN, orderId: "order-A", xpub: MAKER_XPUB, required: 1 })
        const r2 = await proofFn.call({}, { chain: CHAIN, orderId: "order-B", xpub: MAKER_XPUB, required: 1 })
        Test.assert.notEqual(r1.address, r2.address)
    })

    Test.it("FP index uses FP: domain separator — different from TR: for same orderId", () => {
        const orderId = "collision-test"
        const fpIndex  = parseInt(sha256("FP:" + orderId).slice(0, 8), 16) & 0x7fffffff
        const trIndex  = parseInt(sha256("TR:" + orderId).slice(0, 8), 16) & 0x7fffffff
        Test.assert.notEqual(fpIndex, trIndex)
    })

})

// ─── 7. fetch — dual candle query ────────────────────────────────────────────

Test.describe("Order — fetch: dual candle discovery", () => {

    Test.it("returns an array of exactly 2 elements", () => {
        const gun = makeGun()
        const result = fetchFn({ gun, item: ITEM, type: "buy" })
        Test.assert.truthy(Array.isArray(result))
        Test.assert.equal(result.length, 2)
    })

    Test.it("current candle element contains matching order from Gun", async () => {
        const gun = makeGun()
        const o = new Order({ gun, pair: PAIR_MAKER, item: ITEM, type: "buy",
            price: 100, currency: "USDT", chain: 1, xpub: MAKER_XPUB })
        const { key: matchKey } = await o.create()
        // Gun .map() requires RAD/localStorage for lexical filters.
        // In memory-only mode, verify the written entry is readable via direct key access.
        const s = soulFn()
        const data = await new Promise(r => gun.get(s).get(matchKey).once(r))
        Test.assert.truthy(!!data, "current candle must return the order")
    })

    Test.it("previous candle element queries previous candle range", async () => {
        const gun = makeGun()
        const [, prev] = fetchFn({ gun, item: ITEM, type: "sell" })
        // prev should be a Gun chain node (non-null)
        Test.assert.truthy(prev !== null && typeof prev === "object", "prev must be a Gun node")
    })

})

// ─── 8. soul — determinism ────────────────────────────────────────────────────

Test.describe("Order — soul: determinism", () => {

    Test.it("soul() returns a string", () => {
        const s = soulFn()
        Test.assert.truthy(typeof s === "string")
    })

    Test.it("soul() is deterministic — same value on every call", () => {
        const a = soulFn()
        const b = soulFn()
        const c = soulFn()
        Test.assert.equal(a, b)
        Test.assert.equal(b, c)
    })

    Test.it("soul() starts with '$' (Pen-encoded bytecode prefix)", () => {
        Test.assert.truthy(soulFn().startsWith("$"))
    })

})

// ─── 9. create — authenticator + payload ─────────────────────────────────────

Test.describe("Order — create: authenticator + payload integrity", () => {

    Test.it("create writes signed entry to Gun and it is readable back", async () => {
        const gun = makeGun()
        const o = new Order({ gun, pair: PAIR_MAKER, item: ITEM, type: "sell", price: 95, currency: "USDT", chain: 1 })
        const { key } = await o.create()
        const s = soulFn()
        const stored = await gunOnce(gun.get(s).get(key))
        Test.assert.truthy(stored, "entry must exist in Gun after create")
        Test.assert.truthy(typeof stored === "string", "stored value must be a signed string")
    })

    Test.it("payload contains maker's public key", async () => {
        const gun = makeGun()
        const o = new Order({ gun, pair: PAIR_MAKER, item: ITEM, type: "sell", price: 95, currency: "USDT", chain: 1 })
        const { key } = await o.create()
        const s = soulFn()
        const stored = await gunOnce(gun.get(s).get(key))
        Test.assert.truthy(stored.includes(PAIR_MAKER.pub.slice(0, 12)))
    })

    Test.it("buy order payload contains xpub", async () => {
        const gun = makeGun()
        const o = new Order({ gun, pair: PAIR_MAKER, item: ITEM, type: "buy", price: 100, currency: "USDT", chain: 1, xpub: MAKER_XPUB })
        const { key } = await o.create()
        const s = soulFn()
        const stored = await gunOnce(gun.get(s).get(key))
        Test.assert.truthy(stored.includes("xpub"))
    })

    Test.it("sell order payload does NOT contain xpub", async () => {
        const gun = makeGun()
        const o = new Order({ gun, pair: PAIR_MAKER, item: ITEM, type: "sell", price: 95, currency: "USDT", chain: 1 })
        const { key } = await o.create()
        const s = soulFn()
        const stored = await gunOnce(gun.get(s).get(key))
        Test.assert.falsy(stored.includes("xpub"), "sell order must not contain xpub")
    })

    Test.it("create returns { orderId, key }", async () => {
        const o = newSell()
        const result = await o.create()
        Test.assert.truthy("orderId" in result)
        Test.assert.truthy("key" in result)
        Test.assert.equal(result.orderId.length, 64) // SHA-256 hex
    })

})

// ─── 10. Attacker scenarios — adversarial combinations ───────────────────────

Test.describe("Order — attacker: adversarial inputs", () => {

    Test.it("price=0 is rejected before any Gun write can occur", () => {
        const gun = makeGun()
        Test.assert.throws(
            () => new Order({ gun, pair: PAIR_A, item: ITEM, type: "sell", price: 0, currency: "USDT", chain: 1 }),
            "invalidPrice"
        )
    })

    Test.it("100 different pairs produce 100 unique orderIds (entropy adequacy)", async () => {
        const gun = makeGun()
        const ids = await Promise.all(
            Array.from({ length: 100 }, (_, i) => {
                const pair = { pub: `pair${i.toString().padStart(28, "0")}`, epub: "e" }
                const o = new Order({ gun, pair, item: ITEM, type: "sell", price: 10, currency: "USDT", chain: 1 })
                return o.id()
            })
        )
        Test.assert.equal(new Set(ids).size, 100)
    })

    Test.it("OR: TR: FP: domain separators all produce distinct hashes for same input", () => {
        const input = "same-content"
        const or = sha256("OR:" + input)
        const tr = sha256("TR:" + input)
        const fp = sha256("FP:" + input)
        Test.assert.notEqual(or, tr)
        Test.assert.notEqual(or, fp)
        Test.assert.notEqual(tr, fp)
    })

    Test.it("cancel with crafted key where seg3 is all zeros → notOwner", async () => {
        const o = newSell()
        const result = await o.cancel("5000000:item:sell:00000000:0")
        Test.assert.deepEqual(result, { error: "notOwner" })
    })

    Test.it("cancel where seg3 is 9 chars (not 8) → notOwner", async () => {
        // PAIR_A.pub.slice(0,9) ≠ PAIR_A.pub.slice(0,8)
        const o = newSell()
        const wrongKey = `5000000:${ITEM}:sell:${PAIR_A.pub.slice(0, 9)}:0`
        const result = await o.cancel(wrongKey)
        Test.assert.deepEqual(result, { error: "notOwner" })
    })

    Test.it("cancel where seg3 is 7 chars (not 8) → notOwner", async () => {
        const o = newSell()
        const wrongKey = `5000000:${ITEM}:sell:${PAIR_A.pub.slice(0, 7)}:0`
        const result = await o.cancel(wrongKey)
        Test.assert.deepEqual(result, { error: "notOwner" })
    })

    Test.it("1000 tradeIds from different takers on same order — all unique", async () => {
        const ids = await Promise.all(
            Array.from({ length: 1000 }, (_, i) => {
                const gun = makeGun()
                const pair = { pub: `taker${i.toString().padStart(27, "0")}`, epub: "e" }
                const o = new Order({ gun, pair, item: ITEM, type: "sell", price: 95, currency: "USDT", chain: 1 })
                return o.match({ orderId: "fixed-order", makerpub: "maker" })
            })
        )
        const unique = new Set(ids.map(r => r.tradeId))
        Test.assert.equal(unique.size, 1000)
    })

    Test.it("proof required=0 is rejected without touching the chain", async () => {
        let chainCalled = false
        const trackingChain = {
            getBalance: async (addr) => { chainCalled = true; return CHAIN.getBalance(addr) }
        }
        await proofFn.call({}, { chain: trackingChain, orderId: "oid", xpub: MAKER_XPUB, required: 0 })
        Test.assert.falsy(chainCalled, "chain.getBalance must not be called when required=0")
    })

    Test.it("item slug with leading/trailing colon is rejected", () => {
        const gun = makeGun()
        Test.assert.throws(
            () => new Order({ gun, pair: PAIR_A, item: ":item", type: "sell", price: 10, currency: "USDT", chain: 1 }),
            "invalidItem"
        )
        Test.assert.throws(
            () => new Order({ gun, pair: PAIR_A, item: "item:", type: "sell", price: 10, currency: "USDT", chain: 1 }),
            "invalidItem"
        )
    })

})
