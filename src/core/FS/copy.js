import { fs, BROWSER, opfs } from "./shared.js"
import { join } from "./join.js"
import { ensure } from "./ensure.js"

/**
 * Copy files or directories from source to destination
 * Pure copy - preserves original files without any transformation
 * @param {string[]} src - Source path segments
 * @param {string[]} dest - Destination path segments
 * @returns {Promise<{success: boolean, path: string}|undefined>} Result object or undefined
 */
export async function copy(src, dest) {
    if (BROWSER) {
        if (!opfs) return
        try {
            // Detect if source is a directory
            let isDir = false
            try {
                await opfs.$dir(opfs._path(src))
                isDir = true
            } catch {}

            if (!isDir) {
                let buf
                try {
                    buf = await opfs.read(src)
                } catch {
                    console.error("Source path doesn't exist:", Array.isArray(src) ? src.join("/") : src)
                    return
                }
                await opfs.write(dest, buf)
                return { success: true, path: dest.join("/") }
            }

            // Directory copy: recursive walk via handle entries
            const walk = async (srcPath, destPath) => {
                const handle = await opfs.$dir(opfs._path(srcPath))
                for await (const [name, entry] of handle.entries()) {
                    const childSrc = [...srcPath, name]
                    const childDest = [...destPath, name]
                    if (entry.kind === "directory") await walk(childSrc, childDest)
                    else {
                        const buf = await opfs.read(childSrc)
                        await opfs.write(childDest, buf)
                    }
                }
            }
            await walk(src, dest)
            return { success: true, path: dest.join("/") }
        } catch (error) {
            console.error("Error copying:", error)
        }
        return
    }

    if (!fs) {
        console.error("File system not available in browser environment")
        return
    }

    try {
        const srcPath = join(src)
        const destPath = join(dest)

        // Check if source exists
        if (!fs.existsSync(srcPath)) {
            console.error("Source path doesn't exist:", srcPath)
            return
        }

        const stats = fs.statSync(srcPath)

        // If source is a file, copy it directly
        if (stats.isFile()) {
            const destDir = join(dest.slice(0, -1))
            // Ensure destination directory exists
            await ensure(destDir)

            // Copy file as-is without any transformation
            fs.copyFileSync(srcPath, destPath)
            return { success: true, path: destPath }
        }

        // If source is a directory, copy recursively
        if (stats.isDirectory()) {
            // Ensure destination directory exists
            await ensure(destPath)

            // Read all files and subdirectories in the source directory
            const entries = fs.readdirSync(srcPath, { withFileTypes: true })

            // Copy each entry recursively
            for (const entry of entries) {
                const newSrc = [...src, entry.name]
                const newDest = [...dest, entry.name]

                await copy(newSrc, newDest)
            }
            return { success: true, path: destPath }
        }
    } catch (error) {
        console.error("Error copying:", error)
    }
}
