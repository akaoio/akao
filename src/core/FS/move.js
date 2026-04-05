import { fs, BROWSER, opfs } from "./shared.js"
import { join } from "./join.js"

/**
 * Move/rename a file or directory
 * @param {string[]} src - Source path segments
 * @param {string[]} dest - Destination path segments
 * @returns {Promise<boolean>} True if moved successfully, false on error
 */
export async function move(src, dest) {
    if (BROWSER) {
        if (!opfs) return false
        try {
            await opfs.move(src, dest)
            return true
        } catch (error) {
            console.error("Error moving:", error)
            return false
        }
    }
    if (!fs) return false
    try {
        fs.renameSync(join(src), join(dest))
        return true
    } catch (error) {
        console.error("Error moving:", error)
        return false
    }
}
