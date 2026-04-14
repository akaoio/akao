import { Access } from "/core/Access.js"
import zen, { once, userSoul } from "/core/ZEN.js"

export const SOCIAL_PLATFORMS = [
    { key: "twitch",    label: "Twitch",    url: (u) => `https://twitch.tv/${u}` },
    { key: "discord",   label: "Discord",   url: null },
    { key: "reddit",    label: "Reddit",    url: (u) => `https://reddit.com/u/${u}` },
    { key: "steam",     label: "Steam",     url: (u) => `https://steamcommunity.com/id/${u}` },
    { key: "youtube",   label: "YouTube",   url: (u) => `https://youtube.com/@${u}` },
    { key: "instagram", label: "Instagram", url: (u) => `https://instagram.com/${u}` },
    { key: "x",         label: "X",         url: (u) => `https://x.com/${u}` }
]

export class Logic {
    static hashcode(str) {
        let hash = 0
        for (let i = 0; i < str.length; i++) {
            const char = str.charCodeAt(i)
            hash = (hash << 5) - hash + char
            hash = hash & hash
        }
        return Math.abs(hash)
    }

    static authenticated() { return Access.get("authenticated") }

    static async identiconseedfor(avatarId) {
        const seed = Access.get("seed")
        if (!seed) return null
        const hashed = await zen.work(seed, "avatar")
        const idSeed = await zen.work(hashed, avatarId)
        const h1 = Logic.hashcode(idSeed) % 360
        const h2 = (h1 + 150) % 360
        return { idSeed, h1, h2 }
    }

    static async identiconseed() {
        return Logic.identiconseedfor(Access.get("avatar")?.id ?? 0)
    }

    static async loadname() {
        const pair = Access.get("pair")
        if (!pair) return null
        const raw = await once(zen.get(userSoul(pair.pub)).get("name"))
        return typeof raw === "string" ? raw : ""
    }

    static async savename(value) {
        value = value.trim()
        const pair = Access.get("pair")
        if (!pair || !value) return null
        zen.get(userSoul(pair.pub)).get("name").put(value, null, { opt: { authenticator: pair } })
        return value
    }

    static async loadbio() {
        const pair = Access.get("pair")
        if (!pair) return null
        const raw = await once(zen.get(userSoul(pair.pub)).get("bio"))
        return typeof raw === "string" ? raw : ""
    }

    static async savebio(value) {
        value = value.trim().slice(0, 360)
        const pair = Access.get("pair")
        if (!pair) return null
        zen.get(userSoul(pair.pub)).get("bio").put(value, null, { opt: { authenticator: pair } })
        return value
    }

    static async loadlinks() {
        const pair = Access.get("pair")
        if (!pair) return null
        const raw = await once(zen.get(userSoul(pair.pub)).get("links"))
        let links = {}
        if (typeof raw === "string")
            try { links = JSON.parse(raw) } catch { links = {} }
        const allowed = SOCIAL_PLATFORMS.map((p) => p.key)
        return Object.fromEntries(Object.entries(links).filter(([k]) => allowed.includes(k)))
    }

    static async savelinks(obj) {
        const pair = Access.get("pair")
        if (!pair) return null
        const allowed = SOCIAL_PLATFORMS.map((p) => p.key)
        const clean = Object.fromEntries(
            Object.entries(obj)
                .filter(([k, v]) => allowed.includes(k) && typeof v === "string" && v.trim() && /^[a-zA-Z0-9._-]{1,64}$/.test(v.trim()))
                .map(([k, v]) => [k, v.trim()])
        )
        zen.get(userSoul(pair.pub)).get("links").put(JSON.stringify(clean), null, { opt: { authenticator: pair } })
        return clean
    }

    static loadfollowing(callback) {
        const pair = Access.get("pair")
        if (!pair) return null
        const scope = zen.get(userSoul(pair.pub)).get("following").map()
        scope.on(callback)
        return scope
    }

    static addfollow(pub, name, list) {
        pub = pub.trim()
        if (!pub) return null
        name = name.trim() || `${pub.slice(0, 8)}…`
        if (list.some((f) => f.pub === pub)) return null
        const pair = Access.get("pair")
        if (!pair) return null
        zen.get(userSoul(pair.pub)).get("following").get(pub).put({ name, at: Date.now() }, null, { opt: { authenticator: pair } })
        return { pub, name }
    }

    static removefollow(pub) {
        const pair = Access.get("pair")
        if (!pair) return
        zen.get(userSoul(pair.pub)).get("following").get(pub).put(null, null, { opt: { authenticator: pair } })
    }
}

export default Logic
