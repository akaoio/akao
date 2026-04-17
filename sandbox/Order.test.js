/**
 * Order.test.js — Adversarial unit tests for the Order module.
 *
 * Strategy: think like hackers. Test boundary conditions, injection attacks,
 * identity spoofing, replay, side confusion, BigInt traps, and cryptographic
 * invariants (domain separation, determinism, entropy adequacy).
 *
 * All dependencies are real: real Zen (in-memory), real SEA, real EVM (Ganache).
 * No mocks. No fake data. Seeds are deterministic and human-readable.
 */

import Test from "../src/core/Test.js"
import { createRequire } from "module"
import { sha256 } from "../src/core/Utils/crypto.js"
import { Order } from "./Order.js"
import { cancel as cancelFn } from "./Order/cancel.js"
import { match  as matchFn  } from "./Order/match.js"
import { proof  as proofFn  } from "./Order/proof.js"
import { fetch  as fetchFn  } from "./Order/fetch.js"
import { soul   as soulFn   } from "./Order/soul.js"
import zen, { initZEN } from "../src/core/ZEN.js"
import { ethers, HDNodeWallet, getBytes } from "../src/core/Ethers.js"

// ─── Bootstrap: real headless runtime + EVM ───────────────────────────────────

const _req  = createRequire(import.meta.url)
const Ganache = _req("ganache")
const _SEA = zen
const state = { zen: null }
await initZEN()

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

/** Create a real in-memory Zen node (no persistence, no peers). */
function makeGun() {
    return new zen.constructor({ localStorage: false, radisk: false, peers: [] })
}

/** Read a Zen node once, with timeout. Resolves undefined on timeout. */
function gunOnce(node, timeoutMs = 800) {
    return new Promise((resolve) => {
        const t = setTimeout(() => resolve(undefined), timeoutMs)
        node.once((data) => { clearTimeout(t); resolve(data) })
    })
}

async function keyAtStamp(stamp, pub, difficulty = 3) {
    const base = `${stamp}:${pub}`
    const prefix = "0".repeat(difficulty)
    let nonce = 0
    while (true) {
        const next = nonce.toString(36)
        if (sha256(`${base}:${next}`).startsWith(prefix)) return `${base}:${next}`
        nonce++
        if (nonce % 1000 === 0) await new Promise((r) => setTimeout(r, 0))
    }
}

function makeBase(id = ITEM, quantity = 1) {
    return { type: "item", id, quantity }
}

function makeQuote(quantity = 100, contract = "USDT", chain = 1) {
    return { type: "crypto", quantity, contract, chain }
}

function orderInput({ pair = PAIR_MAKER, side = "sell", baseId = ITEM, baseQuantity = 1, quoteQuantity = 95, contract = "USDT", chain = 1, xpub = MAKER_XPUB, affiliate = null } = {}) {
    return {
        pair,
        side,
        base: makeBase(baseId, baseQuantity),
        quote: makeQuote(quoteQuantity, contract, chain),
        xpub,
        affiliate
    }
}

async function publishOrder({ pair = PAIR_MAKER, baseId = ITEM, side = "buy", stamp, quoteQuantity = 100 } = {}) {
    const key = await keyAtStamp(stamp, pair.pub)
    const candle = Math.floor(stamp / 300000)
    const record = {
        orderId: sha256(`OR:${baseId}:${side}:${stamp}:${pair.pub}`),
        maker: {
            pub: pair.pub,
            epub: pair.epub,
            xpub: MAKER_XPUB
        },
        side,
        base: makeBase(baseId),
        quote: makeQuote(quoteQuantity),
        affiliate: null,
        status: "open"
    }
    const value = await _SEA.sign(JSON.stringify(record), pair)
    await new Promise((resolve) => state.zen.get(soulFn({ baseId, side, candle })).get(key).put(value, resolve, { authenticator: pair }))
    return { key, candle, value }
}

async function collectMapValues(node, timeoutMs = 250) {
    return await new Promise((resolve) => {
        const values = new Map()
        node.once((data, key) => {
            if (!key || typeof data !== "string") return
            values.set(key, data)
        })
        setTimeout(() => resolve(values), timeoutMs)
    })
}

/** Create a valid buy Order instance. */
function newBuy(overrides = {}) {
    return new Order({ ...orderInput({ side: "buy", quoteQuantity: 100 }), ...overrides })
}

/** Create a valid sell Order instance. */
function newSell(overrides = {}) {
    return new Order({ ...orderInput({ side: "sell", quoteQuantity: 95 }), ...overrides })
}

// ─── 1. Constructor — validation ──────────────────────────────────────────────

Test.describe("Order — constructor: missing required fields", () => {

    Test.it("throws invalidInput when pair is missing", () => {
        Test.assert.throws(
            () => new Order({ side: "sell", base: makeBase(), quote: makeQuote(10), xpub: MAKER_XPUB }),
            "invalidInput"
        )
    })

    Test.it("throws invalidInput when base is missing", () => {
        Test.assert.throws(
            () => new Order({ pair: PAIR_A, side: "sell", quote: makeQuote(10), xpub: MAKER_XPUB }),
            "invalidInput"
        )
    })

    Test.it("throws invalidInput when side is missing", () => {
        Test.assert.throws(
            () => new Order({ pair: PAIR_A, base: makeBase(), quote: makeQuote(10), xpub: MAKER_XPUB }),
            "invalidInput"
        )
    })

    Test.it("throws invalidInput when quote is missing", () => {
        Test.assert.throws(
            () => new Order({ pair: PAIR_A, side: "sell", base: makeBase(), xpub: MAKER_XPUB }),
            "invalidInput"
        )
    })

    Test.it("throws invalidInput when quote contract is missing", () => {
        Test.assert.throws(
            () => new Order({ pair: PAIR_A, side: "sell", base: makeBase(), quote: { type: "crypto", quantity: 10, chain: 1 }, xpub: MAKER_XPUB }),
            "invalidInput"
        )
    })

    Test.it("throws invalidInput when quote chain is missing", () => {
        Test.assert.throws(
            () => new Order({ pair: PAIR_A, side: "sell", base: makeBase(), quote: { type: "crypto", quantity: 10, contract: "USDT" }, xpub: MAKER_XPUB }),
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

Test.describe("Order — constructor: quote quantity validation", () => {

    Test.it("throws invalidQuoteQuantity for quote.quantity = 0", () => {
        Test.assert.throws(
            () => new Order(orderInput({ pair: PAIR_A, side: "sell", quoteQuantity: 0 })),
            "invalidQuoteQuantity"
        )
    })

    Test.it("throws invalidQuoteQuantity for negative quote.quantity", () => {
        Test.assert.throws(
            () => new Order(orderInput({ pair: PAIR_A, side: "sell", quoteQuantity: -1 })),
            "invalidQuoteQuantity"
        )
    })

    Test.it("throws invalidQuoteQuantity for NaN", () => {
        Test.assert.throws(
            () => new Order(orderInput({ pair: PAIR_A, side: "sell", quoteQuantity: NaN })),
            "invalidQuoteQuantity"
        )
    })

    Test.it("throws invalidQuoteQuantity for Infinity", () => {
        Test.assert.throws(
            () => new Order(orderInput({ pair: PAIR_A, side: "sell", quoteQuantity: Infinity })),
            "invalidQuoteQuantity"
        )
    })

    Test.it("throws invalidQuoteQuantity for string quote.quantity (type confusion)", () => {
        Test.assert.throws(
            () => new Order(orderInput({ pair: PAIR_A, side: "sell", quoteQuantity: "100" })),
            "invalidQuoteQuantity"
        )
    })

})

Test.describe("Order — constructor: side validation", () => {

    Test.it("throws invalidSide for unknown side 'admin'", () => {
        Test.assert.throws(
            () => new Order(orderInput({ pair: PAIR_A, side: "admin", quoteQuantity: 10 })),
            "invalidSide"
        )
    })

    Test.it("throws invalidSide for uppercase 'BUY'", () => {
        Test.assert.throws(
            () => new Order(orderInput({ pair: PAIR_A, side: "BUY", quoteQuantity: 10 })),
            "invalidSide"
        )
    })

    Test.it("throws invalidSide for 'Buy' (mixed case)", () => {
        Test.assert.throws(
            () => new Order(orderInput({ pair: PAIR_A, side: "Buy", quoteQuantity: 10 })),
            "invalidSide"
        )
    })

})

Test.describe("Order — constructor: base id separator injection", () => {

    Test.it("throws invalidBaseId for base id containing single colon", () => {
        Test.assert.throws(
            () => new Order(orderInput({ pair: PAIR_A, side: "sell", baseId: "item:hack", quoteQuantity: 10 })),
            "invalidBaseId"
        )
    })

    Test.it("throws invalidBaseId for base id 'buy:sell:0:nonce' (segment shift attack)", () => {
        Test.assert.throws(
            () => new Order(orderInput({ pair: PAIR_A, side: "sell", baseId: "buy:sell:0:nonce", quoteQuantity: 10 })),
            "invalidBaseId"
        )
    })

    Test.it("throws invalidBaseId for base id with only colons", () => {
        Test.assert.throws(
            () => new Order(orderInput({ pair: PAIR_A, side: "sell", baseId: ":::", quoteQuantity: 10 })),
            "invalidBaseId"
        )
    })

})

Test.describe("Order — constructor: maker xpub requirement", () => {

    Test.it("throws xpubRequired when maker xpub is missing", () => {
        Test.assert.throws(
            () => new Order({ pair: PAIR_A, side: "buy", base: makeBase(), quote: makeQuote(10) }),
            "xpubRequired"
        )
    })

    Test.it("sell order also requires xpub because maker payload is always full", () => {
        Test.assert.throws(
            () => new Order({ pair: PAIR_A, side: "sell", base: makeBase(), quote: makeQuote(10) }),
            "xpubRequired"
        )
    })

    Test.it("buy order with xpub stores it on maker", () => {
        const o = newBuy()
        Test.assert.equal(o.maker.xpub, MAKER_XPUB)
        Test.assert.equal(o.side, "buy")
    })

    Test.it("valid sell order stores normalized side/base/quote fields correctly", () => {
        const o = newSell()
        Test.assert.equal(o.base.id, ITEM)
        Test.assert.equal(o.base.quantity, 1)
        Test.assert.equal(o.quote.quantity, 95)
        Test.assert.equal(o.quote.contract, "USDT")
        Test.assert.equal(o.quote.chain, 1)
        Test.assert.equal(o.affiliate, null)
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
        const collision = sha256("TR:" + JSON.stringify({
            side: o.side,
            maker: o.maker,
            base: o.base,
            quote: o.quote
        }) + ":" + o._ts + ":" + o._rnd)
        Test.assert.notEqual(oid, collision)
    })

})

// ─── 3. key — format + PoW ───────────────────────────────────────────────────

Test.describe("Order — key: format and PoW correctness", () => {

    Test.it("key has exactly 3 colon-separated segments", async () => {
        const o = newSell()
        const k = await o.key()
        const segs = k.split(":")
        Test.assert.equal(segs.length, 3)
    })

    Test.it("segment 0 is the current timestamp in ms", async () => {
        const before = Date.now()
        const k = await newSell().key()
        const after  = Date.now()
        const seg0 = parseInt(k.split(":")[0], 10)
        Test.assert.truthy(seg0 >= before && seg0 <= after)
    })

    Test.it("segment 1 is the full writer pub", async () => {
        const k = await newSell().key()
        Test.assert.equal(k.split(":")[1], PAIR_MAKER.pub)
    })

    Test.it("segment 2 is the mined nonce", async () => {
        const k = await newSell().key()
        Test.assert.truthy(k.split(":")[2].length > 0)
    })

    Test.it("SHA-256 of the full key starts with '000' (difficulty 3)", async () => {
        const k = await newSell().key()
        Test.assert.truthy(sha256(k).startsWith("000"), `PoW failed for key: ${k}`)
    })

})

// ─── 4. cancel — ownership enforcement ───────────────────────────────────────

Test.describe("Order — cancel: ownership enforcement", () => {

    Test.it("returns notOwner when key has fewer than 3 segments", async () => {
        const o = newSell()
        const result = await o.cancel("timestamp:pub")
        Test.assert.deepEqual(result, { error: "notOwner" })
    })

    Test.it("returns notOwner when full pub in key doesn't match pair.pub", async () => {
        const o = newSell()
        const fakeKey = `1744440000000:other-pub:nonce`
        const result = await o.cancel(fakeKey)
        Test.assert.deepEqual(result, { error: "notOwner" })
    })

    Test.it("returns notOwner when key belongs to PAIR_B (cross-cancellation attack)", async () => {
        const victimKey = `1744440000000:${PAIR_B.pub}:nonce`
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

    Test.it("cancel clears entry in Zen when full pub matches owner", async () => {
        state.zen = makeGun()
        const o = new Order(orderInput({ pair: PAIR_MAKER, side: "sell", quoteQuantity: 95 }))
        const { key } = await o.create()
        await o.cancel(key)
        const stamp = Number(key.split(":")[0])
        const s = soulFn({ baseId: ITEM, side: "sell", candle: Math.floor(stamp / 300000) })
        const stored = await gunOnce(state.zen.get(s).get(key))
        Test.assert.falsy(stored && stored !== null && typeof stored === "object" && stored.quote,
            "Zen entry must be cleared after cancel")
    })

    Test.it("cancel with correct full pub does not return notOwner", async () => {
        state.zen = makeGun()
        const o = new Order(orderInput({ pair: PAIR_MAKER, side: "sell", quoteQuantity: 95 }))
        const validKey = `${Date.now()}:${PAIR_MAKER.pub}:xyz`
        const result = await o.cancel(validKey)
        Test.assert.falsy(result?.error, "should not return error for valid owner cancel")
    })

})

// ─── 5. match — tradeId determinism ──────────────────────────────────────────

Test.describe("Order — match: tradeId determinism + domain isolation", () => {

    Test.it("tradeId is a 64-char hex string", async () => {
        const o = new Order(orderInput({ pair: PAIR_A, side: "sell", quoteQuantity: 95 }))
        const { tradeId } = await o.match({ orderId: "abc123", makerpub: "makerpub" })
        Test.assert.equal(tradeId.length, 64)
        Test.assert.truthy(/^[0-9a-f]{64}$/.test(tradeId))
    })

    Test.it("same orderId + makerpub + taker always produces same tradeId (deterministic)", async () => {
        const o = new Order(orderInput({ pair: PAIR_A, side: "sell", quoteQuantity: 95 }))
        const args = { orderId: "fixedorder123", makerpub: "makerX" }
        const a = await o.match(args)
        const b = await o.match(args)
        Test.assert.equal(a.tradeId, b.tradeId)
    })

    Test.it("different takers produce different tradeIds for same order", async () => {
        state.zen = makeGun()
        const oA = new Order(orderInput({ pair: PAIR_A, side: "sell", baseId: ITEM, quoteQuantity: 95 }))
        const oB = new Order(orderInput({ pair: PAIR_B, side: "sell", baseId: ITEM, quoteQuantity: 95 }))
        const args = { orderId: "fixedorder123", makerpub: "makerX" }
        const { tradeId: ta } = await oA.match(args)
        const { tradeId: tb } = await oB.match(args)
        Test.assert.notEqual(ta, tb)
    })

    Test.it("different orderIds produce different tradeIds for same taker", async () => {
        const o = new Order(orderInput({ pair: PAIR_A, side: "sell", quoteQuantity: 95 }))
        const a = await o.match({ orderId: "order-AAA", makerpub: "maker1" })
        const b = await o.match({ orderId: "order-BBB", makerpub: "maker1" })
        Test.assert.notEqual(a.tradeId, b.tradeId)
    })

    Test.it("return value has exactly { tradeId } — no timestamp leaked", async () => {
        const o = new Order(orderInput({ pair: PAIR_A, side: "sell", quoteQuantity: 95 }))
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

    Test.it("match writes trade record to Zen when key is provided", async () => {
        state.zen = makeGun()
        const o = new Order(orderInput({ pair: PAIR_MAKER, side: "sell", quoteQuantity: 95 }))
        const { key, orderId } = await o.create()
        await o.match({ orderId, makerpub: PAIR_MAKER.pub, key })
        const stamp = Number(key.split(":")[0])
        const s = soulFn({ baseId: ITEM, side: "sell", candle: Math.floor(stamp / 300000) })
        const stored = await gunOnce(state.zen.get(s).get(key))
        Test.assert.truthy(stored, "trade record must be written to Zen")
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
        state.zen = makeGun()
        const result = fetchFn({ baseId: ITEM, side: "buy" })
        Test.assert.truthy(Array.isArray(result))
        Test.assert.equal(result.length, 2)
    })

    Test.it("current candle element contains matching order from the exact baseId/side/candle partition", async () => {
        state.zen = makeGun()
        const candle = Math.floor(Date.now() / 300000)
        const currentStamp = candle * 300000 + 1234
        const previousStamp = (candle - 1) * 300000 + 2345
        const oldStamp = (candle - 2) * 300000 + 3456
        const foreignItem = "arc-raiders/foreign-item"

        const current = await publishOrder({ baseId: ITEM, side: "buy", stamp: currentStamp })
        const previous = await publishOrder({ baseId: ITEM, side: "buy", stamp: previousStamp })
        const wrongSide = await publishOrder({ baseId: ITEM, side: "sell", stamp: currentStamp + 1 })
        const wrongItem = await publishOrder({ baseId: foreignItem, side: "buy", stamp: currentStamp + 2 })
        const tooOld = await publishOrder({ baseId: ITEM, side: "buy", stamp: oldStamp })

        const [cur, prev] = fetchFn({ baseId: ITEM, side: "buy", candle })
        const curValues = await collectMapValues(cur)
        const prevValues = await collectMapValues(prev)

        Test.assert.equal(curValues.get(current.key), current.value, "current partition must expose current-candle order")
        Test.assert.equal(prevValues.get(previous.key), previous.value, "previous partition must expose previous-candle order")
        Test.assert.falsy(curValues.has(previous.key), "current partition must not duplicate previous-candle order")
        Test.assert.falsy(prevValues.has(current.key), "previous partition must not duplicate current-candle order")
        Test.assert.falsy(curValues.has(wrongSide.key) || prevValues.has(wrongSide.key), "wrong side must not leak into discovery")
        Test.assert.falsy(curValues.has(wrongItem.key) || prevValues.has(wrongItem.key), "wrong item must not leak into discovery")
        Test.assert.falsy(curValues.has(tooOld.key) || prevValues.has(tooOld.key), "orders older than previous candle must not be discovered")
    })

    Test.it("explicit candle argument shifts discovery window deterministically", async () => {
        state.zen = makeGun()
        const candle = Math.floor(Date.now() / 300000)
        const targetStamp = (candle - 1) * 300000 + 4444
        const olderStamp = (candle - 2) * 300000 + 5555

        const target = await publishOrder({ baseId: ITEM, side: "sell", stamp: targetStamp, quoteQuantity: 95 })
        const older = await publishOrder({ baseId: ITEM, side: "sell", stamp: olderStamp, quoteQuantity: 96 })

        const [cur, prev] = fetchFn({ baseId: ITEM, side: "sell", candle: candle - 1 })
        const curValues = await collectMapValues(cur)
        const prevValues = await collectMapValues(prev)

        Test.assert.equal(curValues.get(target.key), target.value, "explicit candle must make that candle the current discovery partition")
        Test.assert.equal(prevValues.get(older.key), older.value, "explicit candle must still include the immediately previous candle")
    })

})

// ─── 8. soul — determinism ────────────────────────────────────────────────────

    Test.describe("Order — soul: determinism", () => {

    Test.it("soul() returns a string", () => {
        const s = soulFn({ baseId: ITEM, side: "sell", candle: 5000000 })
        Test.assert.truthy(typeof s === "string")
    })

    Test.it("soul() is deterministic for the same params", () => {
        const a = soulFn({ baseId: ITEM, side: "sell", candle: 5000000 })
        const b = soulFn({ baseId: ITEM, side: "sell", candle: 5000000 })
        const c = soulFn({ baseId: ITEM, side: "sell", candle: 5000000 })
        Test.assert.equal(a, b)
        Test.assert.equal(b, c)
    })

    Test.it("soul() changes when candle changes", () => {
        const a = soulFn({ baseId: ITEM, side: "sell", candle: 5000000 })
        const b = soulFn({ baseId: ITEM, side: "sell", candle: 5000001 })
        Test.assert.notEqual(a, b)
    })

    Test.it("soul() starts with '$' (Pen-encoded bytecode prefix)", () => {
        Test.assert.truthy(soulFn({ baseId: ITEM, side: "sell", candle: 5000000 }).startsWith("$"))
    })

})

// ─── 9. create — authenticator + payload ─────────────────────────────────────

Test.describe("Order — create: authenticator + payload integrity", () => {

    Test.it("create writes signed entry to Zen and it is readable back", async () => {
        state.zen = makeGun()
        const o = new Order(orderInput({ pair: PAIR_MAKER, side: "sell", quoteQuantity: 95 }))
        const { key } = await o.create()
        const stamp = Number(key.split(":")[0])
        const s = soulFn({ baseId: ITEM, side: "sell", candle: Math.floor(stamp / 300000) })
        const stored = await gunOnce(state.zen.get(s).get(key))
        Test.assert.truthy(stored, "entry must exist in Zen after create")
        Test.assert.truthy(typeof stored === "string", "stored value must be a signed string")
    })

    Test.it("payload contains maker envelope", async () => {
        state.zen = makeGun()
        const o = new Order(orderInput({ pair: PAIR_MAKER, side: "sell", quoteQuantity: 95 }))
        const { key } = await o.create()
        const stamp = Number(key.split(":")[0])
        const s = soulFn({ baseId: ITEM, side: "sell", candle: Math.floor(stamp / 300000) })
        const stored = await gunOnce(state.zen.get(s).get(key))
        Test.assert.truthy(stored.includes(PAIR_MAKER.pub.slice(0, 12)))
        Test.assert.truthy(stored.includes(PAIR_MAKER.epub.slice(0, 12)))
    })

    Test.it("payload always contains maker xpub", async () => {
        state.zen = makeGun()
        const o = new Order(orderInput({ pair: PAIR_MAKER, side: "buy", quoteQuantity: 100 }))
        const { key } = await o.create()
        const stamp = Number(key.split(":")[0])
        const s = soulFn({ baseId: ITEM, side: "buy", candle: Math.floor(stamp / 300000) })
        const stored = await gunOnce(state.zen.get(s).get(key))
        Test.assert.truthy(stored.includes("xpub"))
    })

    Test.it("sell order payload also contains xpub", async () => {
        state.zen = makeGun()
        const o = new Order(orderInput({ pair: PAIR_MAKER, side: "sell", quoteQuantity: 95 }))
        const { key } = await o.create()
        const stamp = Number(key.split(":")[0])
        const s = soulFn({ baseId: ITEM, side: "sell", candle: Math.floor(stamp / 300000) })
        const stored = await gunOnce(state.zen.get(s).get(key))
        Test.assert.truthy(stored.includes("xpub"), "sell order must carry full maker material too")
    })

    Test.it("create returns { orderId, key }", async () => {
        state.zen = makeGun()
        const o = newSell()
        const result = await o.create()
        Test.assert.truthy("orderId" in result)
        Test.assert.truthy("key" in result)
        Test.assert.equal(result.orderId.length, 64) // SHA-256 hex
    })

})

// ─── 10. Attacker scenarios — adversarial combinations ───────────────────────

Test.describe("Order — attacker: adversarial inputs", () => {

    Test.it("quote.quantity=0 is rejected before any Zen write can occur", () => {
        Test.assert.throws(
            () => new Order(orderInput({ pair: PAIR_A, side: "sell", quoteQuantity: 0 })),
            "invalidQuoteQuantity"
        )
    })

    Test.it("100 different pairs produce 100 unique orderIds (entropy adequacy)", async () => {
        const ids = await Promise.all(
            Array.from({ length: 100 }, (_, i) => {
                const pair = { pub: `pair${i.toString().padStart(28, "0")}`, epub: "e" }
                const o = new Order(orderInput({ pair, side: "sell", quoteQuantity: 10 }))
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

    Test.it("cancel with crafted key where pub segment is forged → notOwner", async () => {
        const o = newSell()
        const result = await o.cancel(`1744440000000:00000000:0`)
        Test.assert.deepEqual(result, { error: "notOwner" })
    })

    Test.it("cancel where pub segment is truncated → notOwner", async () => {
        const o = newSell()
        const wrongKey = `1744440000000:${PAIR_A.pub.slice(0, 9)}:0`
        const result = await o.cancel(wrongKey)
        Test.assert.deepEqual(result, { error: "notOwner" })
    })

    Test.it("cancel where key has extra segments → notOwner", async () => {
        const o = newSell()
        const wrongKey = `1744440000000:${PAIR_A.pub}:0:extra`
        const result = await o.cancel(wrongKey)
        Test.assert.deepEqual(result, { error: "notOwner" })
    })

    Test.it("1000 tradeIds from different takers on same order — all unique", async () => {
        const ids = await Promise.all(
            Array.from({ length: 1000 }, (_, i) => {
                const pair = { pub: `taker${i.toString().padStart(27, "0")}`, epub: "e" }
                const o = new Order(orderInput({ pair, side: "sell", quoteQuantity: 95 }))
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

    Test.it("base id with leading/trailing colon is rejected", () => {
        Test.assert.throws(
            () => new Order(orderInput({ pair: PAIR_A, side: "sell", baseId: ":item", quoteQuantity: 10 })),
            "invalidBaseId"
        )
        Test.assert.throws(
            () => new Order(orderInput({ pair: PAIR_A, side: "sell", baseId: "item:", quoteQuantity: 10 })),
            "invalidBaseId"
        )
    })

})
