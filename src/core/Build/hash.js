import { sha256 } from "../Utils.js"
import { write, load, dir, exist, isDirectory, join } from "../FS.js"

// ============ Hash Generation ============

// Set to collect all hash values for the static hash database
const allHashes = new Set()

/**
 * Recursively collect all hash values from .hash files in a directory
 * Used for excluded directories that already have their hash files
 */
async function collectHashesFromDirectory(path = []) {
    const entries = await dir(path)
    if (!entries || entries.length === 0) return 0
    
    let count = 0
    
    for (const entry of entries) {
        const entryPath = [...path, entry]
        
        if (await isDirectory(entryPath)) {
            // Recursively collect from subdirectories
            count += await collectHashesFromDirectory(entryPath)
        } else if (entry.endsWith(".hash")) {
            // Read and collect hash value
            const hashContent = await load(entryPath)
            if (hashContent) {
                allHashes.add(hashContent)
                count++
            }
        }
    }
    
    return count
}

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
        allHashes.add(fileHash) // Collect hash for static database
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
                allHashes.add(hashContent) // Collect hash for static database
            }
        }
    }

    // Generate directory _.hash if there are any hashes to combine
    if (childHashes.length > 0) {
        const combinedHash = sha256(childHashes.sort().join(""))
        await write([...path, "_.hash"], combinedHash)
        hashCount++
        allHashes.add(combinedHash) // Collect hash for static database
    }

    return hashCount
}

/**
 * Generate hash files for all JSON files in build directory
 * Also creates a static hash database in statics/hashes/
 * @param {Array} pathArray - The root path to start hashing
 * @param {Array} excludeDirs - Directories to exclude from hashing (but include their _.hash if exists)
 */
export async function generateHashFiles(pathArray, excludeDirs = []) {
    if (!(await exist(pathArray))) throw new Error(`Build directory not found: ${join(pathArray)}`)

    // Step 1: Collect old hashes (excluding geo hashes which we'll preserve)
    const oldBuildHashes = new Set()
    const hashesPath = [...pathArray, "statics", "hashes"]
    
    if (await exist(hashesPath)) {
        const oldHashFiles = await dir(hashesPath)
        // Load old hashes to track which ones are from build files
        for (const hashFile of oldHashFiles) {
            oldBuildHashes.add(hashFile)
        }
    }

    // Step 2: Clear the hash collection and generate new hashes
    allHashes.clear()

    // Generate all hash files
    const count = await generateHashesRecursive(pathArray, excludeDirs)

    // Step 3: Copy geo hashes from geo/hashes/ if they exist
    let geoHashCount = 0
    const geoHashesPath = ["geo", "hashes"]
    if (await exist(geoHashesPath)) {
        const geoHashFiles = await dir(geoHashesPath)
        for (const hashFile of geoHashFiles) {
            allHashes.add(hashFile)
            geoHashCount++
        }
    }

    // Step 4: Identify obsolete hashes (old build hashes not in new set, excluding geo hashes)
    const obsoleteHashes = new Set()
    for (const oldHash of oldBuildHashes) {
        if (!allHashes.has(oldHash)) {
            obsoleteHashes.add(oldHash)
        }
    }

    // Step 5: Remove obsolete hash files
    for (const obsoleteHash of obsoleteHashes) {
        try {
            const obsoletePath = [...hashesPath, obsoleteHash]
            if (await exist(obsoletePath)) {
                await write(obsoletePath, null) // This should delete the file
                // Use proper delete via fs
                const fs = await import("fs")
                const fullPath = join(obsoletePath)
                if (fs.existsSync(fullPath)) {
                    fs.unlinkSync(fullPath)
                }
            }
        } catch (error) {
            console.error(`Error removing obsolete hash ${obsoleteHash}:`, error.message)
        }
    }

    // Step 6: Create/update static hash database - empty files named by hash value
    for (const hash of allHashes) {
        await write([...pathArray, "statics", "hashes", hash], "")
    }

    return { 
        hashFiles: count, 
        hashDatabase: allHashes.size,
        geoHashes: geoHashCount,
        obsoleteRemoved: obsoleteHashes.size
    }
}
