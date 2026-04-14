import { wave, passkey } from "/core/Access.js"
import zen from "/core/ZEN.js"

export class Logic {
    constructor() {
        this.session = null
    }

    async pair() {
        if (!zen?.pair) return null
        const pair = await zen.pair()
        this.session = pair
        return pair
    }

    async request(waveEl) {
        if (!zen?.pair) throw new Error("ZEN is not available")
        if (!this.session) {
            this.session = await zen.pair()
            if (!this.session?.epub) throw new Error("Unable to create session pair")
        }
        await waveEl.listen()
        await waveEl.send({ "~": this.session.epub, ":": ">" })
    }

    async handle(parsed) {
        if (!parsed || typeof parsed !== "object") return null
        if (parsed[":"] === "!>") return { type: "deny" }
        if (parsed["~"] && parsed["!"] && parsed["@"] && parsed["#"]) {
            if (!zen?.secret || !zen?.decrypt) return null
            const secret = await zen.secret(parsed["~"], this.session)
            const decrypted = await zen.decrypt({ ct: parsed["!"], iv: parsed["@"], s: parsed["#"] }, secret)
            if (!decrypted) return null
            const seed = Array.isArray(decrypted) ? new Uint8Array(decrypted) : decrypted
            return { type: "grant", seed }
        }
        return null
    }

    reset() {
        this.session = null
    }

    epub(value) {
        if (!value || value.length <= 12) return value || ""
        return `${value.slice(0, 5)}...${value.slice(-5)}`
    }

    async wave(seed) {
        return wave({ seed })
    }

    async passkey() {
        return passkey()
    }
}

export default Logic
