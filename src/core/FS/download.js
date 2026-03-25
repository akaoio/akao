import { fs, NODE, BROWSER } from "./shared.js"
import { join } from "./join.js"
import { ensure } from "./ensure.js"

/**
 * Download a file from a remote URL and save it to the filesystem
 * Supports both Node.js (http/https + fs) and browser (fetch + download trigger) environments
 * @param {string} url - The URL to download from
 * @param {string[]|string} path - Path segments or string path where to save the file
 *                                  If ends with extension, treated as filename
 *                                  If directory, filename extracted from URL
 *                                  If not provided, saves to current directory with URL filename
 * @returns {Promise<{success: boolean, path: string}|undefined>} Result object or undefined
 */
export async function download(url, path = []) {
    // Validate URL
    if (!url || typeof url !== "string") {
        console.error("Invalid URL provided to download:", url)
        return
    }

    // Validate URL format
    let urlObj
    try {
        urlObj = new URL(url)
    } catch (error) {
        console.error("Invalid URL format:", url)
        return
    }

    // Normalize path to array
    if (typeof path === "string") path = [path]
    if (!Array.isArray(path)) path = []

    // Extract filename from URL
    const pathname = urlObj.pathname
    const urlFilename = pathname.split("/").filter((s) => s).pop() || "downloaded-file"
    
    // Determine if path ends with a file or is a directory
    const lastSegment = path.at(-1) || ""
    const hasExtension = lastSegment.includes(".")
    const isFile = hasExtension

    let filename
    let dirPath

    if (isFile) {
        // Path ends with filename
        filename = lastSegment
        dirPath = path.slice(0, -1)
    } else {
        // Path is directory, use filename from URL
        filename = urlFilename
        dirPath = path
    }

    // Build full file path
    const dir = join(dirPath)
    const filePath = join([...dirPath, filename])

    // Ensure directory exists
    if (!(await ensure(dir))) {
        console.error("Failed to create directory:", dir)
        return
    }

    // Download based on environment
    if (NODE) {
        // Node.js environment - use fetch + fs
        if (!fs) {
            console.error("File system not available")
            return
        }

        try {
            const response = await fetch(url)
            
            if (!response.ok) {
                console.error(`Failed to download: ${url} (Status: ${response.status})`)
                return
            }

            // Get response as buffer
            const buffer = Buffer.from(await response.arrayBuffer())
            
            // Write buffer to file
            fs.writeFileSync(filePath, buffer)
            
            return { success: true, path: filePath }
        } catch (error) {
            console.error("Error downloading file:", error)
            return
        }
    } else if (BROWSER) 
        // Browser environment - use fetch API
        try {
            const response = await fetch(url)
            
            if (!response.ok) {
                console.error(`Failed to download: ${url} (Status: ${response.status})`)
                return
            }

            const blob = await response.blob()
            
            // Create a download link and trigger download
            const downloadUrl = window.URL.createObjectURL(blob)
            const a = document.createElement("a")
            a.href = downloadUrl
            a.download = filename
            document.body.appendChild(a)
            a.click()
            
            // Cleanup
            window.URL.revokeObjectURL(downloadUrl)
            document.body.removeChild(a)
            
            return { success: true, path: filename }
        } catch (error) {
            console.error("Error downloading from:", url, error)
            return
        }
    
}
