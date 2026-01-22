import { sha256 } from "../Utils.js"
import { write, load, dir, exist, isDirectory, join } from "../FS.js"

// ============ Hash Generation ============

/**
 * Generate hash files recursively from deepest level up
 * Following the spec:
 * - Each .json file gets a .hash file containing just the hash string
 * - Each directory gets a .hash file that is hash of all direct child hashes
 * - All hash values are collected for the static hash database
 * - Skip directories that should be excluded (e.g., geo)
 */
async function generateHashesRecursive(path = [], excludeDirs = []) {
    const entries = await dir(path)
    if (!entries || entries.length === 0) return 0

    const jsonFiles = []
    const subDirs = []
    let hashCount = 0

    // Separate JSON files and directories using fs.statSync
    for (const entry of entries) {
        const entryPath = [...path, entry]

        if (await isDirectory(entryPath)) {
            // Skip excluded directories
            if (!excludeDirs.includes(entry)) {
                subDirs.push(entry)
            }
        } else if (entry.endsWith(".json") && !entry.endsWith(".hash")) {
            jsonFiles.push(entry)
        }
    }

    // First, process subdirectories recursively (deepest first)
    for (const subDir of subDirs) {
        const count = await generateHashesRecursive([...path, subDir], excludeDirs)
        hashCount += count
    }

    // Now generate hash files for all JSON files in current directory
    const childHashes = []

    for (const jsonFile of jsonFiles) {
        // Read file content and hash it
        const fileContent = await load([...path, jsonFile])
        if (!fileContent) continue

        const fileHash = sha256(JSON.stringify(fileContent))
        // Remove .json extension and add .hash (e.g., mimiza.com.json -> mimiza.com.hash)
        const hashFileName = jsonFile.replace(/\.json$/, ".hash")
        await write([...path, hashFileName], fileHash)
        hashCount++
        childHashes.push(fileHash)
    }

    // Add subdirectory hashes (including excluded dirs that already have hashes)
    const allSubDirs = [...subDirs]
    for (const excluded of excludeDirs) {
        const excludedPath = [...path, excluded]
        if (await exist(excludedPath)) {
            allSubDirs.push(excluded)
        }
    }
    
    for (const subDir of allSubDirs) {
        const subDirHashPath = [...path, subDir, "_.hash"]
        if (await exist(subDirHashPath)) {
            const hashContent = await load(subDirHashPath)
            if (hashContent) {
                childHashes.push(hashContent)
            }
        }
    }

    // Generate directory _.hash if there are any hashes to combine
    if (childHashes.length > 0) {
        const combinedHash = sha256(childHashes.sort().join(""))
        await write([...path, "_.hash"], combinedHash)
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
    if (!(await exist(pathArray))) throw new Error(`Build directory not found: ${join(pathArray)}`)

    // Generate all hash files (.hash next to JSON files, _.hash for directories)
    const count = await generateHashesRecursive(pathArray, excludeDirs)

    return { hashFiles: count }
}
