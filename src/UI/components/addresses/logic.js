import DB from "/core/DB.js"
import { Access } from "/core/Access.js"

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
        const { gun, sea } = globalThis
        const scope = gun.get(`~${pair.pub}`).get("addresses").map()
        scope.on(async (data, key) => {
            if (!data) return
            const address = await sea.decrypt(data, pair)
            callback(key, address)
        })
        return scope
    }

    static async defaults(id) {
        const pair = Access.get("pair")
        if (!pair) return {}
        const { gun } = globalThis
        const billing = await gun.get(`~${pair.pub}`).get("billing")
        const shipping = await gun.get(`~${pair.pub}`).get("shipping")
        const encrypted = await gun.get(`~${pair.pub}`).get("addresses").get(id)
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
        const { gun, sea } = globalThis
        const encrypted = await gun.get(`~${pair.pub}`).get("addresses").get(id)
        return sea.decrypt(encrypted, pair)
    }

    static async write(address, { billing, shipping } = {}) {
        const pair = Access.get("pair")
        if (!pair) return
        const { gun, sea } = globalThis
        const encrypted = await sea.encrypt(address, pair)
        const scope = gun.get(`~${pair.pub}`).get("addresses").get(address.id)
        scope.put(encrypted, null, { opt: { authenticator: pair } })
        if (billing) gun.get(`~${pair.pub}`).get("billing").put(scope, null, { opt: { authenticator: pair } })
        if (shipping) gun.get(`~${pair.pub}`).get("shipping").put(scope, null, { opt: { authenticator: pair } })
    }

    static remove(id) {
        const pair = Access.get("pair")
        if (!pair) return
        const { gun } = globalThis
        gun.get(`~${pair.pub}`).get("addresses").get(id).put(null, null, { opt: { authenticator: pair } })
    }
}

export default Logic
