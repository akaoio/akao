import { Access } from "/core/Access.js"
import zen from "/core/ZEN.js"

export class Logic {
    static async seed() {
        const rawSeed = Access.get("seed")
        const id = Access.get("avatar")?.id
        if (!rawSeed || id == null) return null
        if (!zen) return null
        const namespaced = await zen.hash(rawSeed, "avatar")
        return zen.hash(namespaced, id)
    }
}

export default Logic
