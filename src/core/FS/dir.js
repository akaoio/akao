import { fs, BROWSER, opfs } from "./shared.js"
import { join } from "./join.js"

/**
 * Read directory contents and return list of file/folder names
 * @param {string[]} path - Path segments to the directory
 * @returns {Promise<string[]>} Array of file and directory names, or empty array on error
 */
export async function dir(path, pattern = null) {
    if (BROWSER) {
        if (!opfs) return []
        if (!pattern) return opfs.list(path).catch(() => [])
        // Recursive pattern walk via raw FileSystemDirectoryHandle entries
        const results = []
        const walk = async (currentPath) => {
            let handle
            try {
                handle = await opfs.$dir(opfs._path(currentPath))
            } catch {
                return
            }
            for await (const [name, entry] of handle.entries()) {
                const childPath = [...currentPath, name]
                if (entry.kind === "directory") await walk(childPath)
                else if (entry.kind === "file") {
                    const relPath = childPath.join("/")
                    if (pattern.test(relPath)) results.push(relPath)
                }
            }
        }
        await walk(Array.isArray(path) ? path : [path])
        return results
    }

    if (!fs) {
        console.error("File system not available in browser environment")
        return []
    }

    const dirPath = join(path)
    try {
        if (!fs.existsSync(dirPath)) {
            console.error("Directory doesn't exist:", dirPath)
            return []
        }

        const stats = fs.statSync(dirPath)
        if (!stats.isDirectory()) {
            console.error("Path is not a directory:", dirPath)
            return []
        }
        // If no pattern provided, return immediate children (non-recursive)
        if (!pattern) return fs.readdirSync(dirPath)

        const results = []

        const walk = async (baseItems, prefix = "") => {
            const currentPath = join(baseItems)
            if (!fs.existsSync(currentPath)) return
            const entries = fs.readdirSync(currentPath, { withFileTypes: true })
            for (const entry of entries) {
                const nextItems = [...baseItems, entry.name]
                const relPath = prefix ? `${prefix}/${entry.name}` : entry.name
                if (entry.isDirectory()) await walk(nextItems, relPath)
                else if (entry.isFile() && pattern.test(relPath)) results.push(relPath)
            }
        }

        await walk(path, "")
        return results
    } catch (error) {
        console.error("Error reading directory:", dirPath, error)
        return []
    }
}
