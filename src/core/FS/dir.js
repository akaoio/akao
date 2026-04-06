import { driver } from "./shared.js"

export async function dir(path, pattern = null) {
    if (!Array.isArray(path)) path = path.split("/").filter(Boolean)
    if (!pattern) return driver.list(path)

    const results = []
    const walk = async (currentPath) => {
        for (const { name, isDir } of await driver.entries(currentPath)) {
            const childPath = [...currentPath, name]
            if (isDir) await walk(childPath)
            else if (pattern.test(childPath.join("/"))) results.push(childPath.join("/"))
        }
    }
    await walk(path)
    return results
}
