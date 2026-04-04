import DB from "/core/DB.js"

export class Logic {
    static label(key) {
        return key.replace(/([A-Z])/g, " $1").replace(/^./, (c) => c.toUpperCase()).trim()
    }

    static display(meta = {}) {
        return meta.type || meta.itemTypeName || meta.configs?.type || ""
    }

    static async meta(id) {
        return DB.get(["statics", "items", id, "meta.json"])
    }

    static async locale(id, code) {
        return DB.get(["statics", "items", id, `${code}.json`])
    }
}

export default Logic
