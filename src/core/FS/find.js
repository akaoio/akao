import { driver } from "./shared.js"

export async function find(paths) {
    if (typeof paths === "string") paths = [paths]
    for (const path of paths) {
        const p = Array.isArray(path) ? path : path.split("/").filter(Boolean)
        if (await driver.exists(p)) return p
    }
    throw new Error(`Could not find path in: ${paths.join(", ")}`)
}
