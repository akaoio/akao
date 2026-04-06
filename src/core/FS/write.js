import { YAML, BROWSER, driver } from "./shared.js"
import { stringify as stringifyCSV } from "../CSV.js"

/**
 * Write content to a file — serializes based on extension, then delegates I/O to writeRaw.
 * @param {string[]} path - Path segments including filename
 * @param {*} content - Content to write
 * @returns {Promise<{success: boolean, path: string}|undefined>}
 */
export async function write(path = [], content) {
    if (content === undefined || content === null) return
    const file = path.at(-1)

    if (!file.includes(".") && typeof content === "object" && !(content instanceof String)) {
        console.error("Attempted to write object/array to path without extension:", path.join("/"))
        return
    }

    // Binary: pass through directly, no serialization needed
    if (content instanceof Uint8Array) return driver.writeBytes(path, content)

    // Serialize to string based on file extension
    let data
    const ext = file.slice(file.lastIndexOf(".") + 1).toLowerCase()
    if (ext === "json")                    data = JSON.stringify(content, null, 4)
    else if (ext === "csv")                data = stringifyCSV(content, { delimiter: "," })
    else if (ext === "tsv")                data = stringifyCSV(content, { delimiter: "\t" })
    else if (ext === "yaml" || ext === "yml")
        // Browser has no YAML library — JSON is valid YAML so readers will parse it correctly
        data = !BROWSER ? YAML.stringify(content) : typeof content === "string" ? content : JSON.stringify(content, null, 4)
    else data = typeof content === "string" ? content : JSON.stringify(content, null, 4)

    return driver.writeBytes(path, new TextEncoder().encode(data))
}
