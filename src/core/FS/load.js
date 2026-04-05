import { fs, YAML, NODE, BROWSER, opfs } from "./shared.js"
import { join } from "./join.js"
import { parse as parseCSV } from "../CSV.js"

/**
 * Load content from files or directories (JSON, YAML, or plain text)
 * Supports loading single files, entire directories, or nested object structures
 * @param {string|string[]|object} path - Path segments, single path, or object of paths
 * @returns {Promise<*>} Loaded content (parsed for JSON/YAML, raw for others)
 */
export async function load(path) {
    const content = {}
    // Normalize single string to array
    if (typeof path == "string") path = [path]
    // Handle array of path segments (file or directory path)
    if (Array.isArray(path)) {
        const _path = join(path)
        let text
        // Browser environment - OPFS directory check first, then HTTP + OPFS fallback for files
        if (BROWSER) {
            if (opfs) {
                let isOPFSDir = false
                try {
                    await opfs.$dir(opfs._path(path))
                    isOPFSDir = true
                } catch {}

                if (isOPFSDir) {
                    const names = await opfs.list(path).catch(() => [])
                    const files = {}
                    for (const name of names) {
                        const childContent = await load([...path, name])
                        if (childContent !== undefined) {
                            const base = name.replace(/\.\w{2,4}$/, "")
                            files[base] = childContent
                        }
                    }
                    return files
                }
            }

            // File load: HTTP first, OPFS fallback
            const textExts = ["json", "yaml", "yml", "csv", "tsv", "txt", "md", "html", "js", "css", "hash"]
            const fileExt = _path.match(/\.\w+$/)?.[0]?.slice(1).toLowerCase() || ""
            const isBinary = fileExt && !textExts.includes(fileExt)

            let httpText = null
            try {
                const response = await fetch(_path)
                if (response.ok) {
                    if (isBinary) {
                        const buf = await response.arrayBuffer()
                        if (opfs) opfs.write(path, new Uint8Array(buf)).catch(() => {})
                        return new Uint8Array(buf)
                    }
                    httpText = await response.text()
                    // Background: cache to OPFS — best-effort, failures are expected (quota etc.)
                    if (opfs) {
                        const encoded = new TextEncoder().encode(httpText)
                        opfs.write(path, encoded).catch(() => {})
                    }
                }
            } catch {}

            if (isBinary) {
                const buf = await opfs?.read(path).catch(() => null)
                if (buf) return new Uint8Array(buf)
                console.error("Path not found in HTTP or OPFS:", _path)
                return
            }

            if (httpText !== null) text = httpText
            else if (opfs) {
                // Fallback: serve from OPFS if HTTP failed
                const buf = await opfs.read(path).catch(() => null)
                if (buf) text = new TextDecoder().decode(buf)
                else {
                    console.error("Path not found in HTTP or OPFS:", _path)
                    return
                }
            } else {
                console.error("Path doesn't exist", _path)
                return
            }
        }
        // Node.js environment - use fs module for file operations
        else if (NODE)
            try {
                if (!fs.existsSync(_path)) return console.error("Path doesn't exist", _path)
                // Check if _path is a directory
                const stats = fs.statSync(_path)
                if (stats.isDirectory()) {
                    // Load all files from the directory recursively
                    const files = {}
                    const entries = fs.readdirSync(_path, { withFileTypes: true })
                    // Iterate through directory entries
                    for (const entry of entries) {
                        const content = await load([...path, entry.name])
                        if (content) {
                            const base = entry.name.replace(/\.\w{2,4}$/, "")
                            files[base] = content
                        }
                    }
                    return files
                }
                const textExts = ["json", "yaml", "yml", "csv", "tsv", "txt", "md", "html", "js", "css", "hash"]
                const fileExt = _path.match(/\.\w+$/)?.[0]?.slice(1).toLowerCase() || ""
                if (textExts.includes(fileExt)) text = fs.readFileSync(_path, "utf8")
                else return new Uint8Array(fs.readFileSync(_path))
            } catch (error) {
                console.error("Error reading from", _path)
                return
            }

        if (typeof text === "string") text = text.trim()
        let ext =
            _path
                .match(/\.\w+$/)?.[0]
                ?.slice(1)
                .toLowerCase() || ""
        // Parse JSON, YAML, or CSV files
        if (["json", "yaml", "yml", "csv", "tsv"].includes(ext))
            try {
                let data
                if (ext === "json") data = JSON.parse(text)
                else if (YAML && ["yaml", "yml"].includes(ext)) data = YAML.parse(text)
                else if (["csv", "tsv"].includes(ext)) {
                    const delimiter = ext === "tsv" ? "\t" : ","
                    data = parseCSV(text, { delimiter })
                }
                // Return ABI property if present, otherwise return full data
                return data?.abi !== undefined ? data.abi : data
            } catch {
                // If parsing fails, return raw text
                return text
            }

        // Return raw text for other file types
        return text
    }
    // Handle object input - load multiple paths as key-value pairs
    if (typeof path === "object" && path !== null && !Array.isArray(path)) {
        // Process all entries in parallel for better performance
        const promises = Object.entries(path).map(async ([key, value]) => {
            // Recursively handle nested objects
            if (typeof value === "object" && value !== null && !Array.isArray(value)) content[key] = await load(value)
            else content[key] = await load(value)
        })
        await Promise.all(promises)
    }
    return content
}
