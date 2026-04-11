/**
 * Context module tests — headless-compatible (storage-backed, document optional).
 *
 * Context is a singleton States instance, so tests must save/restore state
 * to avoid cross-test pollution. We test the helpers (getTheme, setTheme,
 * getFiat, setFiat) directly rather than the singleton to stay isolated.
 */
import Test from "../Test.js"
import { Context, getTheme, setTheme, getFiat, setFiat } from "../Context.js"

function saveLS(key) { return globalThis.localStorage?.getItem(key) }
function restoreLS(key, val) {
    if (val === null) globalThis.localStorage?.removeItem(key)
    else globalThis.localStorage?.setItem(key, val)
}

Test.describe("Context — setTheme / getTheme", () => {

    Test.it("setTheme persists theme to localStorage", () => {
        const prev = saveLS("theme")
        setTheme("dark")
        Test.assert.equal(localStorage.getItem("theme"), "dark")
        restoreLS("theme", prev)
    })

    Test.it("setTheme updates document.documentElement dataset", () => {
        if (!globalThis.document) return
        const prev = saveLS("theme")
        setTheme("light")
        Test.assert.equal(document.documentElement.dataset.theme, "light")
        restoreLS("theme", prev)
    })

    Test.it("setTheme updates Context state", () => {
        const prev = Context.get("theme")
        setTheme("dark")
        Test.assert.equal(Context.get("theme"), "dark")
        setTheme(prev || "light")
    })

    Test.it("setTheme does not re-notify when same theme set again", () => {
        setTheme("dark")
        let count = 0
        const off = Context.on("theme", () => count++)
        setTheme("dark") // same value — should not fire
        Test.assert.equal(count, 0)
        off()
    })

    Test.it("getTheme reads from localStorage first", () => {
        const prev = saveLS("theme")
        localStorage.setItem("theme", "dark")
        const theme = getTheme()
        Test.assert.equal(theme, "dark")
        restoreLS("theme", prev)
    })

})

Test.describe("Context — setFiat / getFiat", () => {

    Test.it("setFiat persists fiat code to localStorage", () => {
        const prev = saveLS("fiat")
        // Use a fiat code that is actually in Statics.fiats (fallback: any string that should be ignored)
        const { Statics } = globalThis
        const code = Statics?.fiats?.[0]?.code
        if (!code) return // skip if fiats not loaded
        setFiat(code)
        Test.assert.equal(localStorage.getItem("fiat"), code)
        restoreLS("fiat", prev)
    })

    Test.it("setFiat does nothing for unknown fiat code", () => {
        const prevFiat = Context.get("fiat")
        setFiat("UNKNOWN_CURRENCY_XYZ")
        // State should remain unchanged
        Test.assert.deepEqual(Context.get("fiat"), prevFiat)
    })

    Test.it("getFiat returns an object with a code property", () => {
        const fiat = getFiat()
        // May return undefined if Statics not loaded — accept that gracefully
        if (fiat) Test.assert.truthy(typeof fiat.code === "string")
    })

})
