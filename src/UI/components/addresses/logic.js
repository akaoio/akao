import DB from "/core/DB.js"
import { Access } from "/core/Access.js"
import zen, { once, userSoul } from "/core/ZEN.js"

export class Logic {
    static pair() { return Access.get("pair") }

    static async countries() {
        const data = await DB.get(["geo", "countries.json"])
        if (!data || !data.length) return {}
        return Object.fromEntries(data.map(c => [c.id, c]))
    }

    static watch(callback) {
        const pair = Access.get("pair")
        if (!pair) return null
        const scope = zen.get(userSoul(pair.pub)).get("addresses").map()
        scope.on(async (data, key) => {
            if (!data) return
            const address = await zen.decrypt(data, pair)
            callback(key, address)
        })
        return scope
    }

    static async defaults(id) {
        const pair = Access.get("pair")
        if (!pair) return {}
        const root = zen.get(userSoul(pair.pub))
        const billing = await once(root.get("billing"))
        const shipping = await once(root.get("shipping"))
        const encrypted = await once(root.get("addresses").get(id))
        return { billing, shipping, encrypted }
    }

    static async area(geoId, countries) {
        if (!geoId) return ""
        let area = ""
        let data = null
        while (data == null || data?.parent) {
            const $id = DB.path(data?.parent || geoId)
            data = await DB.get(["geo", ...$id.with(-1, `${$id.at(-1)}.json`)])
            if (data) {
                if (!data.parent) data.name = countries?.[data.id]?.name || data.name
                area = `${area ? `${area}, ` : ""}${data.name}`
            }
        }
        return area
    }

    static async read(id) {
        const pair = Access.get("pair")
        if (!pair) return {}
        const encrypted = await once(zen.get(userSoul(pair.pub)).get("addresses").get(id))
        return zen.decrypt(encrypted, pair)
    }

    static async write(address, { billing, shipping } = {}) {
        const pair = Access.get("pair")
        if (!pair) return
        const encrypted = await zen.encrypt(address, pair)
        const root = zen.get(userSoul(pair.pub))
        const scope = root.get("addresses").get(address.id)
        scope.put(encrypted, null, { opt: { authenticator: pair } })
        if (billing) root.get("billing").put(scope, null, { opt: { authenticator: pair } })
        if (shipping) root.get("shipping").put(scope, null, { opt: { authenticator: pair } })
    }

    static remove(id) {
        const pair = Access.get("pair")
        if (!pair) return
        zen.get(userSoul(pair.pub)).get("addresses").get(id).put(null, null, { opt: { authenticator: pair } })
    }
}

export default Logic
