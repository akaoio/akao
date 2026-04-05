import { fs, BROWSER, opfs } from "./shared.js"
import { join } from "./join.js"

/**
 * Check if a path is a directory
 * @param {string[]} path - Path segments to check
 * @returns {Promise<boolean>} True if path is a directory, false otherwise
 */
export async function isDirectory(path) {
    if (BROWSER) {
        if (!opfs) return false
        try {
            // $dir navigates the path as a directory — if it resolves, it's a dir
            await opfs.$dir(opfs._path(path))
            return true
        } catch {
            return false
        }
    }
    if (!fs) {
        console.error("File system not available in browser environment")
        return false
    }
    try {
        const filePath = join(path)
        if (!fs.existsSync(filePath)) return false
        const stats = fs.statSync(filePath)
        return stats.isDirectory()
    } catch (error) {
        return false
    }
}
