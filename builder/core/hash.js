import { hash } from "../../src/core/Torrent/hash.js"
import { FS } from "../../src/core/FS.js"

const encoder = new TextEncoder()

// Wrap a bencoded info dict in a minimal torrent container: d4:info<infoBytes>e
function wrapTorrent(infoBytes) {
    const prefix = encoder.encode("d4:info")
    const suffix = encoder.encode("e")
    const out = new Uint8Array(prefix.length + infoBytes.length + suffix.length)
    out.set(prefix, 0)
    out.set(infoBytes, prefix.length)
    out.set(suffix, prefix.length + infoBytes.length)
    return out
}

// ============ Hash Generation ============

/**
 * Generate hash files recursively from deepest level up
 * Following the spec:
 * - Each .json file gets a .hash file containing just the hash string (BEP 3 v1 infohash)
 * - Each directory gets a _.hash file that is a BEP 3 multi-file torrent hash of its JSON files
 * - Skip directories that should be excluded (e.g., geo)
 */
async function generateHashesRecursive(path = [], excludeDirs = []) {
    const entries = await FS.dir(path)
    if (!entries || entries.length === 0) return 0

    const jsonFiles = []
    const subDirs = []
    let hashCount = 0

    for (const entry of entries) {
        const entryPath = [...path, entry]

        if (await FS.isDirectory(entryPath)) {
            if (!excludeDirs.includes(entry)) subDirs.push(entry)
        } else if (entry.endsWith(".json") && !entry.endsWith(".hash")) {
            jsonFiles.push(entry)
        }
    }

    // Sort for deterministic hash ordering across all filesystems
    jsonFiles.sort()

    // Process subdirectories first (deepest first)
    for (const subDir of subDirs) {
        const count = await generateHashesRecursive([...path, subDir], excludeDirs)
        hashCount += count
    }

    // Hash each JSON file; collect file descriptors for directory multi-file hash
    const fileDescriptors = []
    const childHashes = []

    for (const jsonFile of jsonFiles) {
        const fileContent = await FS.load([...path, jsonFile])
        if (!fileContent) continue

        const bytes = encoder.encode(JSON.stringify(fileContent))
        const { v1: fileHash, bytes: infoBytes } = await hash(bytes, jsonFile)

        const hashFileName = jsonFile.replace(/\.json$/, ".hash")
        await FS.write([...path, hashFileName], fileHash)
        const torrentFileName = jsonFile.replace(/\.json$/, ".torrent")
        await FS.write([...path, torrentFileName], wrapTorrent(infoBytes))
        hashCount++
        childHashes.push(fileHash)

        // Collect for multi-file directory hash
        const captured = bytes
        fileDescriptors.push({
            path: [jsonFile],
            length: captured.length,
            stream: () => new ReadableStream({ start(c) { c.enqueue(captured); c.close() } }),
        })
    }

    // Add excluded subdirectory _.hash values for directory combination
    const allSubDirs = [...subDirs]
    for (const excluded of excludeDirs) {
        if (await FS.exist([...path, excluded])) allSubDirs.push(excluded)
    }

    for (const subDir of allSubDirs) {
        const subDirHashPath = [...path, subDir, "_.hash"]
        if (await FS.exist(subDirHashPath)) {
            const hashContent = await FS.load(subDirHashPath)
            if (hashContent) childHashes.push(hashContent)
        }
    }

    // Generate directory _.hash (always) and _.torrent (only when real JSON content exists)
    if (fileDescriptors.length > 0) {
        const { bytes: infoBytes } = await hash(fileDescriptors, path.at(-1) || "root")
        // _.torrent = BT infohash of direct files only — valid seedable torrent
        await FS.write([...path, "_.torrent"], wrapTorrent(infoBytes))
        // _.hash = combined hash of ALL children (direct files + subdirs) — reflects full subtree for change detection
        const combined = encoder.encode(childHashes.sort().join(""))
        const { v1: dirHash } = await hash(combined, path.at(-1) || "root")
        await FS.write([...path, "_.hash"], dirHash)
        hashCount++
    } else if (childHashes.length > 0) {
        // Directory contains only subdirs — hash the sorted child _.hash values for change detection.
        // No _.torrent: there is no real file content at this level to seed.
        const bytes = encoder.encode(childHashes.sort().join(""))
        const { v1 } = await hash(bytes, path.at(-1) || "root")
        await FS.write([...path, "_.hash"], v1)
        hashCount++
    }

    return hashCount
}

/**
 * Generate hash files for all JSON files in build directory
 * Creates .hash files next to JSON files and _.hash for directories
 * @param {Array} pathArray - The root path to start hashing
 * @param {Array} excludeDirs - Directories to exclude from hashing (but include their _.hash if exists)
 */
export async function generateHashFiles(pathArray, excludeDirs = []) {
    if (!(await FS.exist(pathArray))) throw new Error(`Build directory not found: ${FS.join(pathArray)}`)

    const count = await generateHashesRecursive(pathArray, excludeDirs)

    return { hashFiles: count }
}
