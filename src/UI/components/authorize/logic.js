import { Access } from "/core/Access.js"
import zen from "/core/ZEN.js"

export class Logic {
    static abbrev(value) {
        if (!value || value.length <= 12) return value || ""
        return `${value.slice(0, 5)}...${value.slice(-5)}`
    }

    // Detect a signed request string: 86 base62 chars + recovery bit + ':' or '/'
    static isrequest(data) {
        if (typeof data !== "string" || data.length < 89) return false
        return /^[0-9A-Za-z]{86}[01][:/]/.test(data)
    }

    // Recover the requester's pub from signed request (for display)
    static async recover(signedRequest) {
        if (!zen?.recover) return null
        try { return await zen.recover(signedRequest) } catch { return null }
    }

    // Encrypt seed to session pub, sign the result so receiver can recover our pub
    static async encode(signedRequest) {
        const pair = Access.get("pair")
        const seed = Access.get("seed")
        if (!Access.get("authenticated") || !pair || !seed) return null
        if (!zen?.recover || !zen?.verify || !zen?.secret || !zen?.encrypt || !zen?.sign) return null
        try {
            const sessionPub = await zen.recover(signedRequest)
            const content = await zen.verify(signedRequest, sessionPub)
            if (content !== ">") return null
            const secret = await zen.secret(sessionPub, pair)
            const seedData = seed instanceof Uint8Array ? Array.from(seed) : seed
            const enc = await zen.encrypt(seedData, secret)
            return await zen.sign(enc, pair)
        } catch {
            return null
        }
    }

    // Sign the deny signal so receiver can verify identity
    static async deny() {
        const pair = Access.get("pair")
        if (!pair || !zen?.sign) return "!>"
        try { return await zen.sign("!>", pair) } catch { return "!>" }
    }
}

export default Logic
