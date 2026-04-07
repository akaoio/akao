import { driver } from "./shared.js"

export async function dir(path, pattern = null) {
    if (!Array.isArray(path)) path = path.split("/").filter(Boolean)
    if (!pattern) return driver.list(path)

    const results = []
    const walk = async (currentPath, relSegments) => {
        for (const { name, isDir } of await driver.entries(currentPath)) {
            const childAbs = [...currentPath, name]
            const childRel = [...relSegments, name]
            if (isDir) await walk(childAbs, childRel)
            else if (pattern.test(childRel.join("/"))) results.push(childRel.join("/"))
        }
    }
    await walk(path, [])
    return results
}
