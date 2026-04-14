import { Access, setAvatar } from "/core/Access.js"
import zen, { userSoul } from "/core/ZEN.js"

export class Logic {
    static async seed() {
        const rawSeed = Access.get("seed")
        if (!rawSeed) return null
        return zen.work(rawSeed, "avatar")
    }

    static id() {
        return Number(Access.get("avatar")?.id || 0)
    }

    static total() {
        return Number(Access.get("avatar")?.total || 0)
    }

    static setid(value, step, current) {
        value = Number(value)
        const total = value >= current ? Math.ceil((value + 1) / step) * step : current
        setAvatar({ id: value, total: total !== current ? total : undefined })
        const pair = Access.get("pair")
        if (pair) zen.get(userSoul(pair.pub)).get("avatar").put(value, null, { opt: { authenticator: pair } })
        return value
    }

    static settotal(value) {
        setAvatar({ total: Number(value) })
        return Number(value)
    }
}

export default Logic
