import { Access } from "/core/Access.js"
import zen from "/core/ZEN.js"

export class Logic {
    static epub(value) {
        if (!value || value.length <= 12) return value || ""
        return `${value.slice(0, 5)}...${value.slice(-5)}`
    }

    static isrequest(parsed) {
        return parsed?.[":"] === ">" && !!parsed?.["~"]
    }

    static async encode(epub) {
        const pair = Access.get("pair")
        const seed = Access.get("seed")
        if (!Access.get("authenticated") || !pair || !seed || !zen?.secret || !zen?.encrypt) return null
        const secret = await zen.secret(epub, pair)
        const seedData = seed instanceof Uint8Array ? Array.from(seed) : seed
        const encrypted = await zen.encrypt(seedData, secret, null, { raw: true })
        return { "~": pair.epub, "!": encrypted.ct, "@": encrypted.iv, "#": encrypted.s }
    }
}

export default Logic
