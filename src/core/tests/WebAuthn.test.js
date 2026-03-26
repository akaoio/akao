/**
 * WebAuthn tests — browser only, all interactive (require physical gesture from user).
 *
 * These tests register and authenticate using the WebAuthn API.
 * They will NOT auto-run; each test has a [Run] button in the UI.
 */
import Test from "../Test.js"
import { WebAuthn } from "../WebAuthn.js"
import { sha256 } from "../Utils.js"

const webauthn = new WebAuthn({
    rp: { id: location.hostname, name: "Test Suite" }
})

// Store credential across interactive tests in this module
let _credential = null

Test.describe("WebAuthn — passkey lifecycle", () => {

    Test.it("create() — register a new passkey credential", async () => {
        const credential = await webauthn.create({
            name: "test-user",
            displayName: "Test User"
        })
        Test.assert.truthy(credential, "credential should be returned")
        Test.assert.truthy(typeof credential.id === "string", "credential.id should be a string")
        Test.assert.truthy(credential.id.length > 0, "credential.id should not be empty")
        Test.assert.truthy(credential.pub, "credential.pub (public key) should exist")
        // Save for subsequent tests
        _credential = credential
    }, { interactive: true, browser: true })

    Test.it("authenticate() — verify existing passkey", async () => {
        if (!_credential) 
            throw new Error("Run the register test first")
        
        const result = await webauthn.authenticate({ id: _credential.id })
        Test.assert.truthy(result, "authentication result should be truthy")
    }, { interactive: true, browser: true })

    Test.it("sign() — sign data with passkey", async () => {
        if (!_credential) 
            throw new Error("Run the register test first")
        
        const data = "hello-world"
        const result = await webauthn.sign({ id: _credential.id, data })
        Test.assert.truthy(result, "signed result should exist")
    }, { interactive: true, browser: true })

    Test.it("create() produces deterministic hash from same credential", async () => {
        // Verify that sha256 of the credential ID is deterministic
        if (!_credential) 
            throw new Error("Run the register test first")
        
        const h1 = sha256(_credential.id)
        const h2 = sha256(_credential.id)
        Test.assert.equal(h1, h2)
    }, { interactive: true, browser: true })

})
