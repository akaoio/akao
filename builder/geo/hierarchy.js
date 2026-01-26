import { readFileSync } from "fs"

export function loadHierarchyMap(path = "./geo/hierarchy.txt") {
    const map = new Map()
    try {
        const text = readFileSync(path, "utf8")
        for (const line of text.split(/\r?\n/)) {
            if (!line) continue
            const [parentStr, childStr, type] = line.split("\t")
            if (type !== "ADM") continue
            const parent = parseInt(parentStr, 10)
            const child = parseInt(childStr, 10)
            if (Number.isInteger(parent) && Number.isInteger(child)) {
                map.set(child, parent)
            }
        }
    } catch (error) {
        console.warn(`⚠️  Could not load hierarchy.txt: ${error.message}`)
    }
    return map
}

export function loadAdminCode5(path = "./geo/adminCode5.txt") {
    const map = new Map()
    try {
        const text = readFileSync(path, "utf8")
        for (const line of text.split(/\r?\n/)) {
            if (!line) continue
            const [idStr, adm5code] = line.split("\t")
            const id = parseInt(idStr, 10)
            if (!Number.isInteger(id)) continue
            if (adm5code) map.set(id, adm5code)
        }
    } catch (error) {
        console.warn(`⚠️  Could not load adminCode5.txt: ${error.message}`)
    }
    return map
}