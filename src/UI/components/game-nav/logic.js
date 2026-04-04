import DB from "/core/DB.js"

export class Logic {
    static async games() {
        const ids = await DB.get(["statics", "games", "1.json"])
        if (!Array.isArray(ids)) return []
        const metas = await Promise.all(
            ids.map((id) => DB.get(["statics", "games", id, "meta.json"]).then((m) => (m ? { id, ...m } : null)))
        )
        return metas.filter(Boolean)
    }
}

export default Logic
