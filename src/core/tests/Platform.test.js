import Test from "../Test.js"
import { canDeriveDevPlatform, devplatform, patchsiteplatform, siteplatform } from "../Platform.js"

Test.describe("Platform — dev fallback identity", () => {

    Test.it("canDeriveDevPlatform() rejects browser-like contexts without WebCrypto", () => {
        Test.assert.equal(
            canDeriveDevPlatform({ browser: true, scope: { crypto: {} } }),
            false
        )
    })

    Test.it("devplatform() is deterministic for pub, epub", async () => {
        const a = await devplatform()
        const b = await devplatform()

        Test.assert.equal(a.pub, b.pub)
        Test.assert.equal(a.epub, b.epub)
    })

    Test.it("patchsiteplatform() fills missing platform fields in dev mode", async () => {
        const site = {}
        const expected = await devplatform()

        const patched = await patchsiteplatform(site, { dev: true })

        Test.assert.equal(patched.pub, expected.pub)
        Test.assert.equal(patched.epub, expected.epub)
        Test.assert.equal(site.platform.pub, expected.pub)
    })

    Test.it("patchsiteplatform() preserves configured platform fields outside dev fallback", async () => {
        const site = {
            platform: {
                pub: "custom-pub",
                epub: "custom-epub"
            }
        }

        const patched = await patchsiteplatform(site, { dev: false })

        Test.assert.equal(patched.pub, "custom-pub")
        Test.assert.equal(patched.epub, "custom-epub")
    })

    Test.it("siteplatform() skips dev fallback in browser contexts without WebCrypto", async () => {
        const site = {}
        const patched = await siteplatform(site, {
            dev: true,
            browser: true,
            scope: { crypto: {} }
        })

        Test.assert.deepEqual(patched, {})
    })

})
