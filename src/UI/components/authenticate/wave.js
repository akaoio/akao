/**
 * Wave Authentication Protocol — Requester (bên xin) side
 *
 * Wire format:
 *   Request  (B → broadcast): { "~": session.epub, ":": ">" }
 *   Deny     (A → broadcast): { ":": "!>" }
 *   Grant    (A → broadcast): { "~": pair_A.epub, "!": ct, "@": iv, "#": s }
 *
 * ECDH shared secret:
 *   A computes: sea.secret(session.epub, pair_A)
 *   B computes: sea.secret(parsed["~"], session)
 */

export class WaveAuth {
    constructor() {
        this.session = null
    }

    // Send auth request. Reuses pre-generated session pair if available.
    async request(waveEl) {
        const { sea } = globalThis
        if (!sea?.pair) throw new Error("SEA is not available")
        if (!this.session) {
            this.session = await sea.pair()
            if (!this.session?.epub) throw new Error("Unable to create session pair")
        }
        await waveEl.listen()
        await waveEl.send({ "~": this.session.epub, ":": ">" })
    }

    // Handle an incoming parsed message. Returns { type: "deny" }, { type: "grant", seed }, or null.
    async handle(parsed) {
        if (!parsed || typeof parsed !== "object") return null
        if (parsed[":"] === "!>") return { type: "deny" }
        if (parsed["~"] && parsed["!"] && parsed["@"] && parsed["#"]) {
            const { sea } = globalThis
            if (!sea?.secret || !sea?.decrypt) return null
            const secret = await sea.secret(parsed["~"], this.session)
            const decrypted = await sea.decrypt({ ct: parsed["!"], iv: parsed["@"], s: parsed["#"] }, secret)
            if (!decrypted) return null
            // Convert plain Array back to Uint8Array so sea.pair() accepts it
            const seed = Array.isArray(decrypted) ? new Uint8Array(decrypted) : decrypted
            return { type: "grant", seed }
        }
        return null
    }

    reset() {
        this.session = null
    }
}

export default WaveAuth
