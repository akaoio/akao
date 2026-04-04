import DB from "/core/DB.js"

export class Logic {
    static label(key) {
        return key.replace(/([A-Z])/g, " $1").replace(/^./, (c) => c.toUpperCase()).trim()
    }

    static display(meta = {}) {
        return meta.type || meta.itemTypeName || meta.configs?.type || ""
    }

    static async meta(game, id) {
        return DB.get(["statics", "items", game, id, "meta.json"])
    }

    static async locale(game, id, code) {
        return DB.get(["statics", "items", game, id, `${code}.json`])
    }
}

export default Logic
