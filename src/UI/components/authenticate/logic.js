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
        if (!zen?.pair || !zen?.sign) throw new Error("ZEN is not available")
        if (!this.session) {
            this.session = await zen.pair()
            if (!this.session?.pub) throw new Error("Unable to create session pair")
        }
        await waveEl.listen()
        const signed = await zen.sign(">", this.session)
        await waveEl.send(signed)
    }

    async handle(data) {
        if (!data || typeof data !== "string") return null
        if (data === "!>") return { type: "deny" }
        if (!zen?.recover || !zen?.verify || !zen?.secret || !zen?.decrypt) return null
        try {
            const senderPub = await zen.recover(data)
            const content = await zen.verify(data, senderPub)
            if (!content) return null
            if (content === "!>") return { type: "deny" }
            const secret = await zen.secret(senderPub, this.session)
            const decrypted = await zen.decrypt(content, secret)
            if (!decrypted) return null
            const seed = Array.isArray(decrypted) ? new Uint8Array(decrypted) : decrypted
            return { type: "grant", seed }
        } catch {
            return null
        }
    }

    reset() {
        this.session = null
    }

    abbrev(value) {
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
