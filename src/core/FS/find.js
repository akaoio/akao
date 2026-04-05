import { fs, BROWSER, opfs } from "./shared.js"
import { join } from "./join.js"

/**
 * Find the first existing path from a list of possible paths
 * @param {string|string[]|string[][]} paths - Path or array of paths to check
 * @returns {Promise<string|string[]|null>} First existing path, or throws error if none found
 * @throws {Error} If no path exists
 */
export async function find(paths) {
    // Normalize single string to array of candidates
    if (typeof paths === "string") paths = [paths]

    if (BROWSER) {
        if (!opfs) return null
        for (const path of paths) {
            // ["dir/file.json"] would be a single-segment path "dir/file.json" in OPFS
            const p = Array.isArray(path) ? path : path.split("/").filter(Boolean)
            if (await opfs.exist(p).catch(() => false)) return path
        }
        throw new Error(`Could not find path in: ${paths.join(", ")}`)
    }

    if (!fs) return null

    // Check each path in order and return first match
    for (const path of paths) if (fs.existsSync(join(path))) return path

    throw new Error(`Could not find path in: ${paths.join(", ")}`)
}
