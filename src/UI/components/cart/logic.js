import DB from "/core/DB.js"

export class Logic {
    static async item(id, locale) {
        return DB.get(["statics", "items", id, `${locale}.json`])
    }
}

export default Logic
