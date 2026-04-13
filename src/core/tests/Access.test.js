/**
 * Access module tests — headless-compatible (requires storage only).
 */
import Test from "../Test.js"
import { Access, getWallet, setWallet, getAvatar, setAvatar } from "../Access.js"

function saveLS(key) { return globalThis.localStorage?.getItem(key) }
function restoreLS(key, val) {
    if (val === null) globalThis.localStorage?.removeItem(key)
    else globalThis.localStorage?.setItem(key, val)
}

Test.describe("Access — getWallet()", () => {

    Test.it("returns default { id: 0 } when nothing in localStorage", () => {
        const prev = saveLS("wallet")
        localStorage.removeItem("wallet")
        const w = getWallet()
        Test.assert.deepEqual(w, { id: 0 })
        restoreLS("wallet", prev)
    })

    Test.it("returns stored wallet object", () => {
        const prev = saveLS("wallet")
        localStorage.setItem("wallet", JSON.stringify({ id: 3, total: 100 }))
        const w = getWallet()
        Test.assert.equal(w.id, 3)
        Test.assert.equal(w.total, 100)
        restoreLS("wallet", prev)
    })

    Test.it("auto-recovers from malformed JSON", () => {
        const prev = saveLS("wallet")
        localStorage.setItem("wallet", "not-json{{{")
        const w = getWallet()
        // Should return default without throwing
        Test.assert.deepEqual(w, { id: 0 })
        restoreLS("wallet", prev)
    })

})

Test.describe("Access — setWallet()", () => {

    Test.it("does nothing when not authenticated", () => {
        const prevAuth = Access.get("authenticated")
        const prevWallet = saveLS("wallet")
        // Ensure unauthenticated
        Access.set({ authenticated: false })
        const result = setWallet({ id: 99 })
        Test.assert.equal(result, undefined)
        restoreLS("wallet", prevWallet)
        Access.set({ authenticated: prevAuth })
    })

    Test.it("updates wallet when authenticated", () => {
        const prevAuth = Access.get("authenticated")
        const prevWallet = Access.get("wallet")
        const prevLS = saveLS("wallet")
        Access.set({ authenticated: true, wallet: { id: 0 } })
        const result = setWallet({ id: 5, total: 200 })
        Test.assert.truthy(result)
        Test.assert.equal(result.id, 5)
        Test.assert.equal(result.total, 200)
        Test.assert.equal(Access.get("wallet").id, 5)
        // Restore
        Access.set({ authenticated: prevAuth, wallet: prevWallet })
        restoreLS("wallet", prevLS)
    })

    Test.it("persists wallet to localStorage when authenticated", () => {
        const prevAuth = Access.get("authenticated")
        const prevLS = saveLS("wallet")
        Access.set({ authenticated: true, wallet: { id: 0 } })
        setWallet({ id: 7 })
        const stored = JSON.parse(localStorage.getItem("wallet") || "{}")
        Test.assert.equal(stored.id, 7)
        Access.set({ authenticated: prevAuth })
        restoreLS("wallet", prevLS)
    })

})

Test.describe("Access — getAvatar()", () => {

    Test.it("returns default { id: 0 } when nothing in localStorage", () => {
        const prev = saveLS("avatar")
        localStorage.removeItem("avatar")
        const a = getAvatar()
        Test.assert.deepEqual(a, { id: 0 })
        restoreLS("avatar", prev)
    })

    Test.it("auto-recovers from malformed JSON", () => {
        const prev = saveLS("avatar")
        localStorage.setItem("avatar", "!!!invalid")
        const a = getAvatar()
        Test.assert.deepEqual(a, { id: 0 })
        restoreLS("avatar", prev)
    })

})
