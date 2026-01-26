import { dir, exist, isDirectory, load, write } from "../../src/core/FS.js"
import { sha256 } from "../../src/core/Utils.js"

let totalDirHashCount = 0

export async function generateGeoDirectoryHashes(path = [], depth = 0) {
    const entries = await dir(path)
    if (!entries || entries.length === 0) return 0

    const hashFiles = []
    const subDirs = []
    let hashCount = 0

    for (const entry of entries) {
        const entryPath = [...path, entry]

        if (await isDirectory(entryPath)) {
            subDirs.push(entry)
        } else if (entry.endsWith(".hash") && entry !== "_.hash") {
            hashFiles.push(entry)
        }
    }

    for (const subDir of subDirs) {
        const count = await generateGeoDirectoryHashes([...path, subDir], depth + 1)
        hashCount += count
    }

    const childHashes = []

    for (const hashFile of hashFiles) {
        const hashContent = await load([...path, hashFile])
        if (hashContent) {
            childHashes.push(hashContent)
        }
    }

    for (const subDir of subDirs) {
        const subDirHashPath = [...path, subDir, "_.hash"]
        if (await exist(subDirHashPath)) {
            const hashContent = await load(subDirHashPath)
            if (hashContent) {
                childHashes.push(hashContent)
            }
        }
    }

    if (childHashes.length > 0) {
        const combinedHash = sha256(childHashes.sort().join(""))
        await write([...path, "_.hash"], combinedHash)
        hashCount++
        totalDirHashCount++

        if (totalDirHashCount % 500 === 0 || (depth <= 2 && totalDirHashCount % 50 === 0)) {
            const pathStr = path.slice(2).join("/") || "root"
            console.log(`  Processed ${totalDirHashCount.toLocaleString()} directories | Current: ${pathStr}`)
        }
    }

    childHashes.length = 0

    return hashCount
}

export function resetGeoHashCounter() {
    totalDirHashCount = 0
}
