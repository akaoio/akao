import Test from "../Test.js"
import Trade from "../Trade.js"
import { sha256 } from "../Utils/crypto.js"

// ─── State management ─────────────────────────────────────────────────────────

Test.describe("Trade — state management", () => {

    Test.it("initialises with empty state", () => {
        const t = new Trade()
        Test.assert.deepEqual(t.get(), {})
    })

    Test.it("set merges multiple keys", () => {
        const t = new Trade()
        t.set({ a: 1, b: "hello" })
        Test.assert.equal(t.get("a"), 1)
        Test.assert.equal(t.get("b"), "hello")
    })

    Test.it("add stores a single key/value", () => {
        const t = new Trade()
        t.add("x", 42)
        Test.assert.equal(t.get("x"), 42)
    })

    Test.it("del removes a key", () => {
        const t = new Trade()
        t.add("y", "remove-me")
        t.del("y")
        Test.assert.equal(t.get("y"), undefined)
    })

    Test.it("get with no key returns full state", () => {
        const t = new Trade()
        t.set({ k1: "v1", k2: "v2" })
        const all = t.get()
        Test.assert.equal(all.k1, "v1")
        Test.assert.equal(all.k2, "v2")
    })

    Test.it("set is chainable", () => {
        const t = new Trade()
        const result = t.set({ n: 1 })
        Test.assert.truthy(result === t)
    })

    Test.it("add is chainable", () => {
        const t = new Trade()
        const result = t.add("q", 9)
        Test.assert.truthy(result === t)
    })

    Test.it("del is chainable", () => {
        const t = new Trade()
        t.add("z", 0)
        const result = t.del("z")
        Test.assert.truthy(result === t)
    })

})

// ─── Role binding ─────────────────────────────────────────────────────────────

Test.describe("Trade — role binding", () => {

    Test.it("bind stores an actor under a role", () => {
        const t = new Trade()
        t.bind("E", { pub: "escrow-pub" })
        Test.assert.equal(t.roles.get("E").pub, "escrow-pub")
    })

    Test.it("unbind removes the role", () => {
        const t = new Trade()
        t.bind("B", { pub: "buyer-pub" })
        t.unbind("B")
        Test.assert.falsy(t.roles.has("B"))
    })

    Test.it("unbind clears cached key material for the role", () => {
        const t = new Trade()
        t.keys.set("B:E", { secret: "abc" })
        t.keys.set("S:E", { secret: "def" })
        t.unbind("B")
        Test.assert.falsy(t.keys.has("B:E"))
        // unrelated cache entry must survive
        Test.assert.truthy(t.keys.has("S:E"))
    })

    Test.it("bind is chainable", () => {
        const t = new Trade()
        const result = t.bind("S", { pub: "seller" })
        Test.assert.truthy(result === t)
    })

    Test.it("unbind is chainable", () => {
        const t = new Trade()
        t.bind("A", { pub: "aff" })
        const result = t.unbind("A")
        Test.assert.truthy(result === t)
    })

    Test.it("bind no-ops when role or actor is falsy", () => {
        const t = new Trade()
        const result = t.bind(null, { pub: "x" })
        Test.assert.truthy(result === t)
        Test.assert.equal(t.roles.size, 0)
    })

})

// ─── Protocol state transitions ───────────────────────────────────────────────

Test.describe("Trade — protocol state transitions", () => {

    Test.it("open sets status to open", async () => {
        const t = new Trade()
        await t.open()
        Test.assert.equal(t.get("status"), "open")
    })

    Test.it("open merges additional data", async () => {
        const t = new Trade()
        await t.open({ orderId: "ord-1" })
        Test.assert.equal(t.get("status"), "open")
        Test.assert.equal(t.get("orderId"), "ord-1")
    })

    Test.it("lock sets status to locked", () => {
        const t = new Trade()
        t.lock()
        Test.assert.equal(t.get("status"), "locked")
    })

    Test.it("release sets status to released", () => {
        const t = new Trade()
        t.release()
        Test.assert.equal(t.get("status"), "released")
    })

    Test.it("refund sets status to refunded", () => {
        const t = new Trade()
        t.refund()
        Test.assert.equal(t.get("status"), "refunded")
    })

    Test.it("dispute sets status to dispute", () => {
        const t = new Trade()
        t.dispute()
        Test.assert.equal(t.get("status"), "dispute")
    })

    Test.it("settle sets status to settled", () => {
        const t = new Trade()
        t.settle()
        Test.assert.equal(t.get("status"), "settled")
    })

    Test.it("resolve sets status to resolved", () => {
        const t = new Trade()
        t.resolve()
        Test.assert.equal(t.get("status"), "resolved")
    })

    Test.it("lock merges extra data", () => {
        const t = new Trade()
        t.lock({ amount: "100" })
        Test.assert.equal(t.get("amount"), "100")
    })

    Test.it("release is chainable", () => {
        const t = new Trade()
        const result = t.release()
        Test.assert.truthy(result === t)
    })

})

// ─── Snapshot / restore ───────────────────────────────────────────────────────

Test.describe("Trade — snapshot / restore", () => {

    Test.it("snapshot captures id, scope, role, state, and roles", () => {
        const t = new Trade({ id: "trade-1", scope: "order", role: "E" })
        t.bind("E", { pub: "epub", pair: { priv: "epriv" } })
        t.add("status", "open")

        const snap = t.snapshot()
        Test.assert.equal(snap.id, "trade-1")
        Test.assert.equal(snap.scope, "order")
        Test.assert.equal(snap.role, "E")
        Test.assert.equal(snap.state.status, "open")
        Test.assert.equal(snap.roles.E.pub, "epub")
    })

    Test.it("snapshot does not expose key pairs", () => {
        const t = new Trade()
        t.bind("B", { pub: "bpub", pair: { priv: "secret" } })
        const snap = t.snapshot()
        Test.assert.falsy(snap.roles.B.pair)
    })

    Test.it("restore hydrates id, scope, role, state, and roles", () => {
        const t = new Trade()
        t.restore({
            id: "r-1",
            scope: "item",
            role: "B",
            state: { status: "locked", amount: "50" },
            roles: { E: { pub: "epub" }, B: { pub: "bpub" } },
        })
        Test.assert.equal(t.id, "r-1")
        Test.assert.equal(t.scope, "item")
        Test.assert.equal(t.role, "B")
        Test.assert.equal(t.get("status"), "locked")
        Test.assert.equal(t.get("amount"), "50")
        Test.assert.equal(t.roles.get("E").pub, "epub")
    })

    Test.it("snapshot → restore round-trips correctly", () => {
        const original = new Trade({ id: "orig", scope: "escrow", role: "S" })
        original.bind("E", { pub: "epub" })
        original.set({ status: "open", item: "sword" })

        const snap = original.snapshot()
        const copy = new Trade().restore(snap)

        Test.assert.equal(copy.id, "orig")
        Test.assert.equal(copy.get("status"), "open")
        Test.assert.equal(copy.get("item"), "sword")
        Test.assert.equal(copy.roles.get("E").pub, "epub")
    })

    Test.it("restore is chainable", () => {
        const t = new Trade()
        const result = t.restore({})
        Test.assert.truthy(result === t)
    })

})

// ─── Sync ─────────────────────────────────────────────────────────────────────

Test.describe("Trade — sync", () => {

    Test.it("sync merges data into state", () => {
        const t = new Trade()
        t.sync({ x: 1, y: 2 })
        Test.assert.equal(t.get("x"), 1)
        Test.assert.equal(t.get("y"), 2)
    })

    Test.it("sync overwrites existing keys", () => {
        const t = new Trade()
        t.add("v", "old")
        t.sync({ v: "new" })
        Test.assert.equal(t.get("v"), "new")
    })

    Test.it("sync is chainable", () => {
        const t = new Trade()
        const result = t.sync({ a: 1 })
        Test.assert.truthy(result === t)
    })

    Test.it("sync ignores non-objects", () => {
        const t = new Trade()
        t.add("safe", 1)
        t.sync(null)
        t.sync("string")
        Test.assert.equal(t.get("safe"), 1)
    })

})

// ─── Route / emit ─────────────────────────────────────────────────────────────

Test.describe("Trade — route / emit", () => {

    Test.it("route registers handler, emit calls it", () => {
        const t = new Trade()
        let received = null
        t.route("msg", (p) => { received = p })
        t.emit("msg", "hello")
        Test.assert.equal(received, "hello")
    })

    Test.it("route returns unsubscribe function", () => {
        const t = new Trade()
        let count = 0
        const off = t.route("ev", () => count++)
        t.emit("ev", null)
        off()
        t.emit("ev", null)
        Test.assert.equal(count, 1)
    })

    Test.it("emit calls all handlers for the event", () => {
        const t = new Trade()
        const results = []
        t.route("e", (p) => results.push("a:" + p))
        t.route("e", (p) => results.push("b:" + p))
        t.emit("e", "x")
        Test.assert.deepEqual(results, ["a:x", "b:x"])
    })

    Test.it("emit on unknown event does not throw", () => {
        const t = new Trade()
        t.emit("no-such-event", null)
    })

    Test.it("emit is chainable", () => {
        const t = new Trade()
        const result = t.emit("noop", null)
        Test.assert.truthy(result === t)
    })

})

// ─── HD key derivation (root / child) ─────────────────────────────────────────

Test.describe("Trade — root / child derivation", () => {

    Test.it("root returns an HDNodeWallet for a known seed", async () => {
        const t = new Trade()
        const seedHex = sha256("test-seed")
        const node = await t.root(seedHex)
        Test.assert.truthy(node.address)
        Test.assert.truthy(node.privateKey)
    })

    Test.it("root is deterministic — same seed yields same address", async () => {
        const t = new Trade()
        const seedHex = sha256("deterministic-seed")
        const a = await t.root(seedHex)
        const b = await t.root(seedHex)
        Test.assert.equal(a.address, b.address)
    })

    Test.it("root exposes extendedKey (xprv) and neuter xpub", async () => {
        const t = new Trade()
        const node = await t.root(sha256("xpub-test"))
        Test.assert.truthy(node.extendedKey.startsWith("xprv"))
        Test.assert.truthy(node.neuter().extendedKey.startsWith("xpub"))
    })

    Test.it("child derives child from live node at numeric index", async () => {
        const t = new Trade()
        const rootNode = await t.root(sha256("child-test"))
        const c = await t.child(rootNode, 0)
        Test.assert.truthy(c.address)
    })

    Test.it("child accepts xpub string (watch-only path)", async () => {
        const t = new Trade()
        const rootNode = await t.root(sha256("xpub-child"))
        const xpub = rootNode.neuter().extendedKey
        const c = await t.child(xpub, 1)
        Test.assert.truthy(c.address)
    })

    Test.it("child accepts seed hex as index — derives 31-bit non-hardened index", async () => {
        const t = new Trade()
        const rootNode = await t.root(sha256("index-seed"))
        const indexSeed = sha256("order-1")
        const expectedIndex = parseInt(indexSeed.substring(0, 8), 16) & 0x7fffffff
        const byInt = await t.child(rootNode, expectedIndex)
        const bySeed = await t.child(rootNode, indexSeed)
        Test.assert.equal(byInt.address, bySeed.address)
    })

    Test.it("watch-only and full derivation produce identical address (white-paper §6)", async () => {
        const t = new Trade()
        const s = sha256("se-secret" + ":" + "item-42")
        const rootNode = await t.root(s)
        const indexSeed = sha256("be-secret" + ":" + "order-7")

        const addrFromXprv = (await t.child(rootNode, indexSeed)).address
        const addrFromXpub = (await t.child(rootNode.neuter().extendedKey, indexSeed)).address

        Test.assert.equal(addrFromXprv, addrFromXpub)
    })

})

// ─── Seed derivation (sha256-based) ──────────────────────────────────────────

Test.describe("Trade — seed derivation", () => {

    Test.it("seed returns a 64-char hex string when pair secret is pre-cached", async () => {
        const t = new Trade()
        // Pre-cache a mock secret to avoid needing real sea.secret
        t.keys.set("S:E", { secret: "mock-secret-se" })
        const s = await t.seed("S", "E")
        Test.assert.equal(s.length, 64)
        Test.assert.equal(s, sha256("mock-secret-se"))
    })

    Test.it("seed with scope incorporates scope into derivation", async () => {
        const t = new Trade()
        t.keys.set("B:E", { secret: "mock-secret-be" })
        const withoutScope = await t.seed("B", "E")
        const withScope = await t.seed("B", "E", "order-99")
        Test.assert.notEqual(withoutScope, withScope)
        Test.assert.equal(withScope, sha256("mock-secret-be:order-99"))
    })

    Test.it("seed result is cached when no scope is used", async () => {
        const t = new Trade()
        t.keys.set("A:E", { secret: "cached-secret" })
        await t.seed("A", "E")
        Test.assert.truthy(t.keys.get("A:E")?.seed)
    })

    Test.it("seed returns undefined if pair secret cannot be resolved", async () => {
        const t = new Trade()
        // No roles bound, no cached secret — pair() will return undefined
        const s = await t.seed("X", "Y")
        Test.assert.equal(s, undefined)
    })

})
