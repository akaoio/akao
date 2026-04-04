import DB from "/core/DB.js"

export class Logic {
    static async meta() {
        return DB.get(["statics", "items", "meta.json"])
    }

    static async items(start, end, pages) {
        if (end < start) end = start
        if (end > pages) end = pages
        const data = []
        for (let i = start; i <= end; i++) {
            const page = await DB.get(["statics", "items", `${i}.json`])
            if (page) data.push(...page)
        }
        return data
    }
}

export default Logic
