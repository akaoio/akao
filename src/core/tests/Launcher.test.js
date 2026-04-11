import Test from "../Test.js"
import { Progress } from "../Progress.js"
import { Statics } from "../Stores.js"
import { Context } from "../Context.js"

Test.describe("Launcher — isomorphic boot", () => {

    Test.it("boots the core runtime through Launcher.js in Node headless mode", async () => {
        const { launch } = await import("../Launcher.js")
        const state = await launch({ mode: "headless" })

        Test.assert.equal(state.mode, "headless")
        Test.assert.truthy(state.main?.initialized)
        Test.assert.equal(Progress.get("Site"), true)
        Test.assert.equal(Progress.get("GDB"), true)
        Test.assert.equal(Progress.get("Context"), true)
        Test.assert.truthy(!!Statics.site, "site configs must be loaded")
        Test.assert.truthy(Array.isArray(Statics.locales) && Statics.locales.length > 0, "locales must be loaded")
        Test.assert.truthy(!!Context.get("route"), "context route must be initialized")
    })

})
