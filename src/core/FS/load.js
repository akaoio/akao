import { YAML, BROWSER, driver, isBinary } from "./shared.js"
import { join } from "./join.js"
import { parse as parseCSV } from "../CSV.js"

/**
 * Load content from files or directories (JSON, YAML, or plain text).
 * Browser: HTTP-first with OPFS caching fallback.
 * Node.js: direct filesystem read.
 * @param {string|string[]|object} path
 * @returns {Promise<*>} Parsed content, raw text, Uint8Array for binary, or object map for dirs
 */
export async function load(path, options = {}) {
    const quiet = options.quiet === true

    if (typeof path === "string") path = [path]

    if (Array.isArray(path)) {
        const _path = join(path)  // needed for fetch URL (browser) and ext detection
        let text

        if (BROWSER) {
            // Directory: load all children recursively
            if (await driver.isDir(path)) {
                const files = {}
                for (const name of await driver.list(path)) {
                    const child = await load([...path, name], options)
                    if (child !== undefined) files[name.replace(/\.\w{2,4}$/, "")] = child
                }
                return files
            }

            // File: HTTP first, OPFS fallback
            const _isBinary = isBinary(_path)
            let httpText = null
            try {
                const response = await fetch(_path)
                if (response.ok) {
                    if (_isBinary) {
                        const buf = await response.arrayBuffer()
                        driver.writeBytes(path, new Uint8Array(buf)).catch((e) => console.warn("OPFS cache write failed:", e))  // background cache
                        return new Uint8Array(buf)
                    }
                    httpText = await response.text()
                    driver.writeBytes(path, new TextEncoder().encode(httpText)).catch((e) => console.warn("OPFS cache write failed:", e))  // background cache
                }
            } catch {}

            if (_isBinary) {
                const buf = await driver.readBytes(path)
                if (buf) return buf
                if (!quiet) console.error("Path not found in HTTP or OPFS:", _path)
                return
            }

            if (httpText !== null) text = httpText
            else {
                const buf = await driver.readBytes(path)
                if (buf) text = new TextDecoder().decode(buf)
                else {
                    if (!quiet) console.error("Path not found in HTTP or OPFS:", _path)
                    return
                }
            }
        } else {
            // Node.js: direct read
            if (!await driver.exists(path)) {
                if (!quiet) console.error("Path doesn't exist", _path)
                return
            }

            if (await driver.isDir(path)) {
                const files = {}
                for (const { name } of await driver.entries(path)) {
                    const child = await load([...path, name], options)
                    if (child) files[name.replace(/\.\w{2,4}$/, "")] = child
                }
                return files
            }

            if (isBinary(_path)) return await driver.readBytes(path)
            const bytes = await driver.readBytes(path)
            if (!bytes) {
                if (!quiet) console.error("Error reading from", _path)
                return
            }
            text = new TextDecoder().decode(bytes)
        }

        // Deserialize text content
        if (typeof text === "string") text = text.trim()
        const ext = _path.match(/\.\w+$/)?.[0]?.slice(1).toLowerCase() || ""
        if (["json", "yaml", "yml", "csv", "tsv"].includes(ext))
            try {
                let data
                if (ext === "json") data = JSON.parse(text)
                else if (YAML && ["yaml", "yml"].includes(ext)) data = YAML.parse(text)
                else if (["csv", "tsv"].includes(ext))
                    data = parseCSV(text, { delimiter: ext === "tsv" ? "\t" : "," })
                return data?.abi !== undefined ? data.abi : data
            } catch { return text }

        return text
    }

    // Object input: load multiple paths in parallel as key-value pairs
    if (typeof path === "object" && path !== null) {
        const content = {}
        await Promise.all(
            Object.entries(path).map(async ([key, value]) => {
                content[key] = await load(value, options)
            })
        )
        return content
    }
}
