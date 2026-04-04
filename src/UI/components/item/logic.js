import DB from "/core/DB.js"

export class Logic {
    static path(key) {
        const parts = key.includes("/") ? key.split("/") : null
        return {
            meta: parts ? ["statics", "items", ...parts, "meta.json"] : ["statics", "items", key, "meta.json"],
            locale: (code) => parts ? ["statics", "items", ...parts, `${code}.json`] : ["statics", "items", key, `${code}.json`],
            route: parts ? `/item/${parts.join("/")}` : `/item/${key}`
        }
    }

    static async meta(key) {
        const paths = Logic.path(key)
        return DB.get(paths.meta)
    }

    static async locale(key, code) {
        const paths = Logic.path(key)
        return DB.get(paths.locale(code))
    }
}

export default Logic
