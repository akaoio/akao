import DB from "/core/DB.js"

export class Logic {
    static async meta(id) {
        return DB.get(["statics", "tags", id, "meta.json"])
    }

    static async items(id, pages) {
        const results = []
        for (let page = 1; page <= pages; page++) {
            const data = await DB.get(["statics", "tags", id, `${page}.json`])
            if (Array.isArray(data)) results.push(...data)
        }
        return results
    }
}

export default Logic
