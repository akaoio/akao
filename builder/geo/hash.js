import { FS } from "../../src/core/FS.js"
import { hash } from "../../src/core/Torrent/hash.js"

const encoder = new TextEncoder()
let totalDirHashCount = 0

export async function generateGeoDirectoryHashes(path = [], depth = 0) {
    const entries = await FS.dir(path)
    if (!entries || entries.length === 0) return 0

    const hashFiles = []
    const subDirs = []
    let hashCount = 0

    for (const entry of entries) {
        const entryPath = [...path, entry]

        if (await FS.isDirectory(entryPath)) {
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
        const hashContent = await FS.load([...path, hashFile])
        if (hashContent) {
            childHashes.push(hashContent)
        }
    }

    for (const subDir of subDirs) {
        const subDirHashPath = [...path, subDir, "_.hash"]
        if (await FS.exist(subDirHashPath)) {
            const hashContent = await FS.load(subDirHashPath)
            if (hashContent) {
                childHashes.push(hashContent)
            }
        }
    }

    if (childHashes.length > 0) {
        const bytes = encoder.encode(childHashes.sort().join(""))
        const { v1: combinedHash } = await hash(bytes, path.at(-1) || "root")
        await FS.write([...path, "_.hash"], combinedHash)
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
