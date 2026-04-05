import { fs, YAML, BROWSER, opfs } from "./shared.js"
import { join } from "./join.js"
import { ensure } from "./ensure.js"
import { stringify as stringifyCSV } from "../CSV.js"

/**
 * Write content to a file in JSON, YAML, or plain text format
 * @param {string[]} path - Path segments including filename
 * @param {*} content - Content to write (will be serialized based on file extension)
 * @returns {Promise<{success: boolean, path: string}|undefined>} Result object or undefined
 */
export async function write(path = [], content) {
    if (content === undefined || content === null) return
    path = [...path]
    const file = path.at(-1)
    const hasExtension = file.includes(".")

    // Smart detection: treat as file if:
    // 1. Has extension (explicit file like .json, .txt, etc.)
    // 2. Content is string/number/boolean (not object/array) - indicates text file
    // This prevents accidentally writing to directories while supporting extension-less files
    const isFile = hasExtension || typeof content !== "object" || content instanceof String

    if (!isFile) {
        console.error("Attempted to write object/array to path without extension:", join(path))
        return
    }

    // Binary shortcut: Uint8Array written as-is, bypassing text serialization
    if (content instanceof Uint8Array) {
        if (BROWSER) {
            if (!opfs) return
            await opfs.write(path, content)
            return { success: true, path: path.join("/") }
        }
        path.pop()
        const dir = join(path)
        const filePath = join([...path, file])
        if (!(await ensure(dir))) return
        try {
            fs.writeFileSync(filePath, content)
            return { success: true, path: filePath }
        } catch (error) {
            console.error("Error writing to", filePath)
        }
        return
    }

    // Serialize content based on file extension
    let data
    if (file.endsWith(".json")) data = JSON.stringify(content, null, 4)
    else if (file.endsWith(".csv")) data = stringifyCSV(content, { delimiter: "," })
    else if (file.endsWith(".tsv")) data = stringifyCSV(content, { delimiter: "\t" })
    else if (file.endsWith(".yaml") || file.endsWith(".yml"))
        if (!BROWSER) data = YAML.stringify(content)
        else data = typeof content === "string" ? content : JSON.stringify(content, null, 4)
    else data = typeof content === "string" ? content : JSON.stringify(content, null, 4)

    if (BROWSER) {
        if (!opfs) return
        const encoded = new TextEncoder().encode(data)
        await opfs.write(path, encoded)
        return { success: true, path: path.join("/") }
    }

    // If the last item is a file, remove it from path to make dir
    path.pop()
    const dir = join(path)
    const filePath = join([...path, file])
    // Ensure directory exists before writing
    if (!(await ensure(dir))) return

    try {
        fs.writeFileSync(filePath, data, "utf8")
        return { success: true, path: filePath }
    } catch (error) {
        console.error("Error writing to", filePath)
    }
}
