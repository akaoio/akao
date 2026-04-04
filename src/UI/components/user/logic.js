import { Access } from "/core/Access.js"

export class Logic {
    static async seed() {
        const rawSeed = Access.get("seed")
        const id = Access.get("avatar")?.id
        if (!rawSeed || id == null) return null
        const { sea } = globalThis
        if (!sea) return null
        const namespaced = await sea.work(rawSeed, "avatar")
        return sea.work(namespaced, id)
    }
}

export default Logic
