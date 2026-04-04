import DB from "/core/DB.js"

export class Logic {
    static async node(id) {
        const path = DB.path(id)
        return DB.get(["geo", ...path.with(-1, `${path.at(-1)}.json`)])
    }

    static async countries() {
        const data = await DB.get(["geo", "countries.json"])
        if (!data) return []
        return Object.values(data)
            .sort((a, b) => a.name.localeCompare(b.name))
            .map((c) => ({ value: c.id, label: c.name }))
    }

    static async children(id) {
        const path = DB.path(id)
        const data = await DB.get(["geo", ...path.with(-1, `${path.at(-1)}.json`)])
        if (!data?.children?.length) return []
        const options = []
        for (const child of data.children) {
            const $id = DB.path(child)
            const $data = await DB.get(["geo", ...$id.with(-1, `${$id.at(-1)}.json`)])
            if ($data) options.push({ value: $data.id, label: $data.name })
        }
        return options
    }

    static async ancestors(id) {
        const chain = []
        const path = DB.path(id)
        let $data = await DB.get(["geo", ...path.with(-1, `${path.at(-1)}.json`)])
        let selectedId = id
        while ($data?.parent) {
            const parentPath = DB.path($data.parent)
            const parent = await DB.get(["geo", ...parentPath.with(-1, `${parentPath.at(-1)}.json`)])
            if (!parent) break
            chain.unshift({ id: parent.id, name: parent.name, children: parent.children || [], selected: selectedId })
            selectedId = parent.id
            $data = parent
        }
        return chain
    }
}

export default Logic
