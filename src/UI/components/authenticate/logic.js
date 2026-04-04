import { wave, passkey } from "/core/Access.js"

export class Logic {
    constructor() {
        this.session = null
    }

    async pair() {
        const { sea } = globalThis
        if (!sea?.pair) return null
        const pair = await sea.pair()
        this.session = pair
        return pair
    }

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

    async handle(parsed) {
        if (!parsed || typeof parsed !== "object") return null
        if (parsed[":"] === "!>") return { type: "deny" }
        if (parsed["~"] && parsed["!"] && parsed["@"] && parsed["#"]) {
            const { sea } = globalThis
            if (!sea?.secret || !sea?.decrypt) return null
            const secret = await sea.secret(parsed["~"], this.session)
            const decrypted = await sea.decrypt({ ct: parsed["!"], iv: parsed["@"], s: parsed["#"] }, secret)
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
