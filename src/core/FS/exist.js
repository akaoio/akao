import { fs, NODE, BROWSER, opfs } from "./shared.js"
import { join } from "./join.js"

/**
 * Check if a file or directory exists at the given path
 * @param {string|string[]} path - Path segments to check
 * @returns {Promise<boolean>} True if path exists, false otherwise
 */
export async function exist(path = []) {
    if (typeof path === "string") path = [path]
    try {
        if (BROWSER) {
            if (!opfs) return false
            return opfs.exist(path)
        }
        if (NODE) return fs.existsSync(join(path))
    } catch (error) {
        console.error("Error checking file existence:", error)
        return false
    }
}
