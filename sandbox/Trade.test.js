import Test from "../src/core/Test.js"
import { createRequire } from "module"
import { sha256 } from "../src/core/Utils/crypto.js"
import { Trade } from "./Trade.js"
import { Lock } from "./Lock.js"
import { devplatform } from "../src/core/Platform.js"
import zen, { initZEN } from "../src/core/ZEN.js"
import { ethers, HDNodeWallet, getBytes } from "../src/core/Ethers.js"

const _req = createRequire(import.meta.url)
const Ganache = _req("ganache")
const _SEA = zen
await initZEN()

const PLATFORM = await devplatform({ runtime: _SEA })
const [MAKER_PAIR, TAKER_PAIR, AFFILIATE_PAIR] = await Promise.all([
    _SEA.pair(), _SEA.pair(), _SEA.pair()
])

const ganache = Ganache.provider({ logging: { quiet: true } })
const evm = new ethers.BrowserProvider(ganache)
const signer = await evm.getSigner(0)

async function rootFromParty(pair) {
    const secret = await _SEA.secret(PLATFORM.epub, pair)
    return HDNodeWallet.fromSeed(getBytes("0x" + sha256(secret)))
}

const [MAKER_ROOT, TAKER_ROOT, AFFILIATE_ROOT] = await Promise.all([
    rootFromParty(MAKER_PAIR),
    rootFromParty(TAKER_PAIR),
    rootFromParty(AFFILIATE_PAIR)
])

function makeGun() {
    return new zen.constructor({ localStorage: false, radisk: false, peers: [] })
}

async function put(zen, pub, pair, key, value) {
    await new Promise((resolve) => {
        zen.get("~" + pub).get(key).put(value, resolve, { authenticator: pair })
    })
}

async function tradeRecord(zen, pub, tradeId) {
    const data = await zen.get("~" + pub).get("trades").get(tradeId).once(1000)
    if (!data || typeof data !== "object") return data
    const clean = {}
    for (const [key, value] of Object.entries(data))
        if (key !== "_") clean[key] = value
    return clean
}

async function publishProfiles(zen) {
    await Promise.all([
        put(zen, TAKER_PAIR.pub, TAKER_PAIR, "xpub", TAKER_ROOT.neuter().extendedKey),
        put(zen, MAKER_PAIR.pub, MAKER_PAIR, "xpub", MAKER_ROOT.neuter().extendedKey),
        put(zen, AFFILIATE_PAIR.pub, AFFILIATE_PAIR, "xpub", AFFILIATE_ROOT.neuter().extendedKey)
    ])
}

function makeTrade({ zen, orderSide = "buy", tradeOrderId = "order-1", withAffiliate = true } = {}) {
    const order = {
        side: orderSide,
        affiliate: withAffiliate ? { pub: AFFILIATE_PAIR.pub } : null,
        async id() {
            return tradeOrderId
        }
    }

    const maker = { pub: MAKER_PAIR.pub, epub: MAKER_PAIR.epub, pair: MAKER_PAIR, wallet: signer }
    const taker = { pub: TAKER_PAIR.pub, epub: TAKER_PAIR.epub, pair: TAKER_PAIR }
    const platform = { pub: PLATFORM.pub, epub: PLATFORM.epub, pair: PLATFORM.pair }
    const affiliate = { pub: AFFILIATE_PAIR.pub, epub: AFFILIATE_PAIR.epub, pair: AFFILIATE_PAIR }

    return {
        trade: new Trade({ zen, order, maker, taker, platform }),
        maker,
        taker,
        platform,
        affiliate
    }
}

Test.describe("Trade — protocol state helpers", () => {

    Test.it("constructor throws invalidInput when required entities are missing", () => {
        Test.assert.throws(() => new Trade({}), "invalidInput")
    })

    Test.it("id() matches Order.match() tradeId formula and is cached", async () => {
        const zen = makeGun()
        await publishProfiles(zen)
        const { trade } = makeTrade({ zen, tradeOrderId: "order-id-cache" })

        const a = await trade.id()
        const b = await trade.id()
        const expected = sha256("TR:" + "order-id-cache" + ":" + MAKER_PAIR.pub + ":" + TAKER_PAIR.pub)

        Test.assert.equal(a, b)
        Test.assert.equal(a, expected)
    })

    Test.it("create() writes matched state to taker namespace only", async () => {
        const zen = makeGun()
        await publishProfiles(zen)
        const { trade } = makeTrade({ zen, tradeOrderId: "order-create" })

        const created = await trade.create()
        const makerSide = await tradeRecord(zen, MAKER_PAIR.pub, created.tradeId)
        const takerSide = await tradeRecord(zen, TAKER_PAIR.pub, created.tradeId)

        Test.assert.equal(created.writer, "taker")
        Test.assert.equal(makerSide, undefined)
        Test.assert.equal(takerSide.status, "matched")
        Test.assert.equal(takerSide.matched, true)
    })

    Test.it("deposit() fetches published xpubs, funds TL/CL, and records payer state", async () => {
        const zen = makeGun()
        await publishProfiles(zen)
        const { trade, affiliate } = makeTrade({ zen, tradeOrderId: "order-deposit" })

        const result = await trade.deposit({
            affiliate,
            amounts: { tl: 700n, cl: 300n }
        })

        const payerSide = await tradeRecord(zen, MAKER_PAIR.pub, result.tradeId)
        const tlBalance = await evm.getBalance(result.tl)
        const clBalance = await evm.getBalance(result.cl)

        Test.assert.equal(result.txs.length, 2)
        Test.assert.equal(tlBalance, 700n)
        Test.assert.equal(clBalance, 300n)
        Test.assert.equal(payerSide.status, "deposited")
        Test.assert.equal(JSON.parse(payerSide.txs).length, 2)
    })

    Test.it("deposit() surfaces partialDeposit and persists partial state", async () => {
        const zen = makeGun()
        await publishProfiles(zen)
        const { trade, affiliate } = makeTrade({ zen, tradeOrderId: "order-partial" })
        const failingPayer = {
            pub: MAKER_PAIR.pub,
            epub: MAKER_PAIR.epub,
            pair: MAKER_PAIR,
            wallet: {
                sends: 0,
                async sendTransaction({ to, value }) {
                    this.sends++
                    if (this.sends === 2) throw new Error("boom")
                    return {
                        hash: `tx-${to}`,
                        status: 1,
                        async wait() {
                            return { transactionHash: `tx-${to}`, status: 1 }
                        }
                    }
                }
            }
        }

        const result = await trade.deposit({
            payer: failingPayer,
            affiliate,
            amounts: { tl: 10n, cl: 5n }
        })

        const payerSide = await tradeRecord(zen, MAKER_PAIR.pub, result.tradeId)
        Test.assert.equal(result.error, "partialDeposit")
        Test.assert.equal(result.txs.length, 1)
        Test.assert.equal(payerSide.status, "deposit_partial")
    })

    Test.it("deliver() writes seller state and confirm() reveals deterministic unlock indexes", async () => {
        const zen = makeGun()
        await publishProfiles(zen)
        const { trade, taker, affiliate } = makeTrade({ zen, tradeOrderId: "order-confirm" })
        const tradeId = await trade.id()

        const delivered = await trade.deliver()
        const confirmed = await trade.confirm()

        const sellerSide = await tradeRecord(zen, taker.pub, tradeId)
        const buyerLockTL = new Lock({
            payer: MAKER_PAIR,
            platform: PLATFORM.pair,
            recipient: { xpub: TAKER_ROOT.neuter().extendedKey },
            tradeId,
            type: "TL"
        })
        const buyerLockCL = new Lock({
            payer: MAKER_PAIR,
            platform: PLATFORM.pair,
            recipient: { xpub: AFFILIATE_ROOT.neuter().extendedKey },
            tradeId,
            type: "CL"
        })

        Test.assert.equal(delivered.status, "delivered")
        Test.assert.equal(sellerSide.delivered, true)
        Test.assert.equal(confirmed.unlock_index_TL, await buyerLockTL.index())
        Test.assert.equal(confirmed.unlock_index_CL, await buyerLockCL.index())
        Test.assert.truthy(affiliate.pub.length > 0)
    })

    Test.it("release() validates published xpubs, derives unlock wallets, and writes platform state", async () => {
        const zen = makeGun()
        await publishProfiles(zen)
        const { trade, affiliate, platform } = makeTrade({ zen, tradeOrderId: "order-release" })
        const tradeId = await trade.id()

        const released = await trade.release({ affiliate })
        const platformSide = await tradeRecord(zen, platform.pub, tradeId)

        const tl = new Lock({
            payer: MAKER_PAIR,
            platform: PLATFORM.pair,
            recipient: { xpub: TAKER_ROOT.neuter().extendedKey },
            tradeId,
            type: "TL"
        })
        const cl = new Lock({
            payer: MAKER_PAIR,
            platform: PLATFORM.pair,
            recipient: { xpub: AFFILIATE_ROOT.neuter().extendedKey },
            tradeId,
            type: "CL"
        })

        Test.assert.equal(released.status, "release_ready")
        Test.assert.equal(released.tl.address, await tl.address())
        Test.assert.equal(released.cl.address, await cl.address())
        Test.assert.equal(platformSide.status, "release_ready")
    })

    Test.it("refund() derives the same lock addresses and records refund metadata", async () => {
        const zen = makeGun()
        await publishProfiles(zen)
        const { trade, affiliate, platform } = makeTrade({ zen, tradeOrderId: "order-refund" })
        const tradeId = await trade.id()

        const refunded = await trade.refund({ affiliate, to: "0xrefund-destination" })
        const platformSide = await tradeRecord(zen, platform.pub, tradeId)

        const tl = new Lock({
            payer: MAKER_PAIR,
            platform: PLATFORM.pair,
            recipient: { xpub: TAKER_ROOT.neuter().extendedKey },
            tradeId,
            type: "TL"
        })

        Test.assert.equal(refunded.status, "refund_ready")
        Test.assert.equal(refunded.tl.address, await tl.address())
        Test.assert.equal(refunded.refundTo, "0xrefund-destination")
        Test.assert.equal(platformSide.status, "refund_ready")
    })

    Test.it("read() merges maker, taker, and platform namespaces into one state view", async () => {
        const zen = makeGun()
        await publishProfiles(zen)
        const { trade, affiliate } = makeTrade({ zen, tradeOrderId: "order-read" })

        const created = await trade.create()
        await trade.deliver()
        await trade.confirm()
        await trade.release({ affiliate })

        const state = await trade.read({ tradeId: created.tradeId })

        Test.assert.equal(state.status, "release_ready")
        Test.assert.equal(state.delivered, true)
        Test.assert.equal(state.confirmed, true)
        Test.assert.equal(state.releaseReady, true)
    })

})
