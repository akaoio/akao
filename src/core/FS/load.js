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
    const fresh = options.fresh === true

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
            let httpStatus = null
            try {
                const response = await fetch(_path)
                httpStatus = response.status
                if (response.ok) {
                    if (_isBinary) {
                        const buf = await response.arrayBuffer()
                        driver.writeBytes(path, new Uint8Array(buf)).then(() => _prefetchTorrent(path)).catch((e) => console.warn("OPFS cache write failed:", e))
                        return new Uint8Array(buf)
                    }
                    httpText = await response.text()
                    driver.writeBytes(path, new TextEncoder().encode(httpText)).then(() => _prefetchTorrent(path)).catch((e) => console.warn("OPFS cache write failed:", e))
                } else if (fresh && response.status === 404) await driver.remove(path)
            } catch {}

            if (fresh) {
                if (_isBinary) {
                    if (!quiet && httpStatus === 404) console.error("Path not found in HTTP:", _path)
                    return
                }
                if (httpText !== null) text = httpText
                else {
                    if (!quiet && httpStatus === 404) console.error("Path not found in HTTP:", _path)
                    return
                }
            } else if (_isBinary) {
                const buf = await driver.readBytes(path)
                if (buf) return buf

                // Torrent fallback — worker already cached to OPFS
                const torrentData = await _leechFromTorrent(path)
                if (torrentData) return torrentData

                if (!quiet) console.error("Path not found in HTTP or OPFS:", _path)
                return
            }

            if (!fresh && httpText !== null) text = httpText
            else if (!fresh) {
                const buf = await driver.readBytes(path)
                if (buf) text = new TextDecoder().decode(buf)
                else {
                    // Torrent fallback — worker already cached to OPFS
                    const torrentData = await _leechFromTorrent(path)
                    if (torrentData) text = new TextDecoder().decode(torrentData)
                    else {
                        if (!quiet) console.error("Path not found in HTTP or OPFS:", _path)
                        return
                    }
                }
            }
        } else if (await driver.exists(path)) {
            // Node.js: disk first, Torrent fallback (main-thread direct, no worker)
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
        } else {
            // Disk miss → leech directly from in-process Statics.torrent
            const torrentData = await _leechDirect(path)
            if (torrentData) {
                if (isBinary(_path)) return torrentData
                text = new TextDecoder().decode(torrentData)
            } else {
                if (!quiet) console.error("Path not found on disk or P2P:", _path)
                return
            }
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

/**
 * BROWSER mode: leech via torrent worker thread (avoids blocking UI).
 * Worker handles P2P + cache. Main thread reads back from OPFS.
 * Returns Uint8Array on success, null on failure. Timeout: 12s.
 */
async function _leechFromTorrent(path = []) {
    const threads = globalThis.threads
    if (!threads?.threads?.torrent) return null

    return new Promise((resolve) => {
        const timeout = setTimeout(() => resolve(null), 12000)

        threads.queue({
            thread: "torrent",
            method: "leech",
            params: { path },
            callback: (response, error) => {
                clearTimeout(timeout)
                if (error || !response?.success) {
                    resolve(null)
                    return
                }
                driver
                    .readBytes(path)
                    .then((buf) => resolve(buf ? new Uint8Array(buf) : null))
                    .catch(() => resolve(null))
            }
        })
    })
}

/**
 * NODE.js headless mode: leech directly via in-process Statics.torrent.
 * No worker round-trip — torrent client lives in same thread (no UI to block).
 */
async function _leechDirect(path = []) {
    const { Statics } = await import("../Stores.js")
    if (!Statics?.torrent) return null
    const { leech } = await import("../Torrent/leech.js")
    return await leech(Statics.torrent, path)
}

/**
 * Background: auto-seed content after OPFS write completes.
 * Only seeds real content files from public directories (statics/).
 * Skips .hash and .torrent metadata files.
 */
function _prefetchTorrent(path) {
    if (!BROWSER || !Array.isArray(path)) return
    // Only seed from public statics/ directory — prevents data leaks
    if (path[0] !== "statics") return
    const last = path.at(-1)
    if (!last || !last.includes(".")) return
    if (last.endsWith(".hash") || last.endsWith(".torrent")) return
    const threads = globalThis.threads
    if (!threads?.threads?.torrent) return
    threads.queue({ thread: "torrent", method: "seed", params: { path }, callback: () => {} })
}
