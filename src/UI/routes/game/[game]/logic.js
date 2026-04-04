import DB from "/core/DB.js"

export class Logic {
    static sort(items, sort, rarityOrder = []) {
        const copy = [...items]
        if (sort === "name") return copy.sort((a, b) => (a.name || "").localeCompare(b.name || ""))
        if (sort === "name-desc") return copy.sort((a, b) => (b.name || "").localeCompare(a.name || ""))
        if (sort === "value-asc") return copy.sort((a, b) => (a.value || 0) - (b.value || 0))
        if (sort === "value-desc") return copy.sort((a, b) => (b.value || 0) - (a.value || 0))
        if (sort === "rarity-asc" || sort === "rarity-desc") {
            const orderMap = Object.fromEntries(rarityOrder.map((r, i) => [r.toLowerCase(), i]))
            const rank = (item) => orderMap[(item.rarity || "").toLowerCase()] ?? rarityOrder.length
            copy.sort((a, b) => rank(a) - rank(b))
            if (sort === "rarity-desc") copy.reverse()
            return copy
        }
        return copy
    }

    static filter(items, { type, rarity, search } = {}) {
        let result = items
        if (type) result = result.filter((i) => i.type === type)
        if (rarity) result = result.filter((i) => (i.rarity || "").toLowerCase() === rarity.toLowerCase())
        if (search) {
            const q = search.toLowerCase()
            result = result.filter((i) => (i.name || "").toLowerCase().includes(q))
        }
        return result
    }

    static async meta(id) {
        return DB.get(["statics", "games", id, "meta.json"])
    }

    static async info(id, code) {
        return DB.get(["statics", "games", id, `${code}.json`])
    }

    static async catalog(id) {
        return DB.get(["statics", "games", id, "items", "meta.json"])
    }

    static async item(gameId, itemId, locale) {
        const [meta, loc] = await Promise.all([
            DB.get(["statics", "items", gameId, itemId, "meta.json"]),
            DB.get(["statics", "items", gameId, itemId, `${locale}.json`])
        ])
        if (!meta) return null
        const icon = meta.images?.[0] ? `/statics/items/${gameId}/${itemId}/images/${meta.images[0]}` : null
        return { ...meta, ...(loc || {}), id: itemId, icon, catalog: "game" }
    }

    static async page(gameId, pageNum) {
        return DB.get(["statics", "games", gameId, "items", `${pageNum}.json`])
    }
}

export default Logic
