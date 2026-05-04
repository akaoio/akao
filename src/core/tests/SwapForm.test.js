import Test from "../Test.js"
import { Logic } from "../../UI/routes/swap/logic.js"
import { Chains, Lives } from "../Stores.js"

// Derive everything from live runtime data — no mocks
const tokens = Logic.allTokens(Chains, Lives.pools)
const pools = Lives.pools

// Pick two tokens that share at least one chain with a pool between them
const validPair = (() => {
    for (const from of tokens) for (const to of tokens) if (from !== to && Logic.resolveChain(from, to, pools).length > 0) return { from, to }
    return null
})()

// Pick a token that has no pool pairing with validPair.from (for negative cases)
const unpairedToken = tokens.find((t) => t !== validPair?.from && Logic.resolveChain(validPair?.from, t, pools).length === 0)

// ── allTokens ─────────────────────────────────────────────────────────────────

Test.describe("SwapForm - allTokens()", () => {
    Test.it("returns at least one token from live chain data", () => {
        Test.assert.truthy(tokens.length > 0)
    })

    Test.it("each token appears only once — no duplicates by symbol", () => {
        const symbols = tokens.map((t) => t.configs.symbol)
        Test.assert.equal(symbols.length, new Set(symbols).size)
    })

    Test.it("every token has a chains array and _chainAddresses map", () => {
        Test.assert.truthy(tokens.every((t) => Array.isArray(t.chains) && t.chains.length > 0 && typeof t._chainAddresses === "object"))
    })

    Test.it("_chainAddresses keys match the chains array entries", () => {
        Test.assert.truthy(tokens.every((t) => t.chains.every((c) => t._chainAddresses[c] !== undefined)))
    })

    Test.it("returns empty array when Chains is empty", () => {
        Test.assert.equal(Logic.allTokens({}, {}).length, 0)
    })

    Test.it("handles missing pool data gracefully", () => {
        Test.assert.truthy(Logic.allTokens(Chains, null).length > 0)
    })
})

// ── resolveChain ──────────────────────────────────────────────────────────────

Test.describe("SwapForm - resolveChain()", () => {
    Test.it("resolves a valid pair to at least one chain", () => {
        if (!validPair) return
        Test.assert.truthy(Logic.resolveChain(validPair.from, validPair.to, pools).length > 0)
    })

    Test.it("resolved chains are a subset of both tokens' chain lists", () => {
        if (!validPair) return
        const result = Logic.resolveChain(validPair.from, validPair.to, pools)
        Test.assert.truthy(result.every((c) => validPair.from.chains.includes(c) && validPair.to.chains.includes(c)))
    })

    Test.it("returns empty array when no pool exists for the pair", () => {
        if (!validPair || !unpairedToken) return
        Test.assert.equal(Logic.resolveChain(validPair.from, unpairedToken, pools).length, 0)
    })

    Test.it("returns empty array when either token is null", () => {
        if (!validPair) return
        Test.assert.equal(Logic.resolveChain(null, validPair.to, pools).length, 0)
        Test.assert.equal(Logic.resolveChain(validPair.from, null, pools).length, 0)
    })

    Test.it("returns empty array when pools is null", () => {
        if (!validPair) return
        Test.assert.equal(Logic.resolveChain(validPair.from, validPair.to, null).length, 0)
    })

    Test.it("is symmetric — from/to order does not matter", () => {
        if (!validPair) return
        Test.assert.deepEqual(Logic.resolveChain(validPair.from, validPair.to, pools), Logic.resolveChain(validPair.to, validPair.from, pools))
    })
})
