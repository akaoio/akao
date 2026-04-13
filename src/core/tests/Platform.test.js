import Test from "../Test.js"
import { devplatform, patchsiteplatform } from "../Platform.js"

Test.describe("Platform — dev fallback identity", () => {

    Test.it("devplatform() is deterministic for pub, epub, and xpub", async () => {
        const a = await devplatform()
        const b = await devplatform()

        Test.assert.equal(a.pub, b.pub)
        Test.assert.equal(a.epub, b.epub)
        Test.assert.equal(a.xpub, b.xpub)
    })

    Test.it("patchsiteplatform() fills missing platform fields in dev mode", async () => {
        const site = {}
        const expected = await devplatform()

        const patched = await patchsiteplatform(site, { dev: true })

        Test.assert.equal(patched.pub, expected.pub)
        Test.assert.equal(patched.epub, expected.epub)
        Test.assert.equal(patched.xpub, expected.xpub)
        Test.assert.equal(site.platform.pub, expected.pub)
    })

    Test.it("patchsiteplatform() preserves configured platform fields outside dev fallback", async () => {
        const site = {
            platform: {
                pub: "custom-pub",
                epub: "custom-epub",
                xpub: "custom-xpub"
            }
        }

        const patched = await patchsiteplatform(site, { dev: false })

        Test.assert.equal(patched.pub, "custom-pub")
        Test.assert.equal(patched.epub, "custom-epub")
        Test.assert.equal(patched.xpub, "custom-xpub")
    })

})
