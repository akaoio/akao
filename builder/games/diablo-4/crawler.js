/**
 * Diablo 4 Items Crawler
 * Extracts items from Wowhead's D4 items page (server-rendered HTML)
 */

import fs from "node:fs/promises"
import path from "node:path"

async function fileExists(filePath) {
    try {
        await fs.access(filePath)
        return true
    } catch {
        return false
    }
}

function normalizeRawItem(item) {
    return {
        id: item.id,
        icon: item.icon,
        name: item.name,
        itemTypeName: item.itemTypeName,
        quality: item.quality,
        playerClassNames: item.playerClassNames || [],
        slotNames: item.slotNames || [],
        popularity: item.popularity || 0,
    }
}

async function loadItemsFromSavedPages(output) {
    const pagesDir = path.join(output, "pages")
    if (!(await fileExists(pagesDir))) return []

    let files = []
    try {
        files = await fs.readdir(pagesDir)
    } catch {
        return []
    }

    const pageFiles = files
        .filter((name) => /^page-\d+\.json$/.test(name))
        .sort((a, b) => Number(a.match(/\d+/)?.[0] || 0) - Number(b.match(/\d+/)?.[0] || 0))

    if (!pageFiles.length) return []

    const byId = new Map()
    for (const file of pageFiles) {
        try {
            const raw = await fs.readFile(path.join(pagesDir, file), "utf8")
            const parsed = JSON.parse(raw)
            const data = Array.isArray(parsed?.data) ? parsed.data : []
            for (const item of data) {
                if (item?.id == null) continue
                byId.set(item.id, normalizeRawItem(item))
            }
        } catch {
            // ignore corrupt page file and continue
        }
    }

    return [...byId.values()]
}

async function loadExistingItems(output) {
    const filePath = path.join(output, "items.json")
    if (!(await fileExists(filePath))) return []

    try {
        const raw = await fs.readFile(filePath, "utf8")
        const parsed = JSON.parse(raw)
        if (!Array.isArray(parsed)) return []
        return parsed.filter((item) => item?.id != null)
    } catch {
        return []
    }
}

export async function crawlDiablo4Items(options = {}) {
    const output = options.output ? path.resolve(options.output) : path.resolve("games", "diablo-4")
    const dryRun = !!options.dryRun
    const maxPages = options.maxPages || 5
    const limit = options.limit || Infinity
    const concurrency = options.concurrency || 8

    console.log(`[D4] Starting crawler for ${maxPages} page(s)...`)
    console.log(`[D4] Dry run: ${dryRun}, Limit: ${limit}, Concurrency: ${concurrency}`)

    const items = []
    const seenItemIds = new Set()
    const imageUrls = new Map() // maps iconId to URL
    const fetchedAt = new Date().toISOString()
    let pagesFetched = 0
    let duplicateItemsSkipped = 0
    let previousPageIds = null

    try {
        const pagesDir = `${output}/pages`

        // Crawl pages (starting from page 0)
        for (let pageNum = 0; pageNum < maxPages; pageNum++) {
            if (items.length >= limit) break

            console.log(`[D4] Fetching page ${pageNum}...`)
            const pageUrl = `https://www.wowhead.com/diablo-4/items${pageNum > 0 ? `?page=${pageNum}` : ""}`

            const pageItems = await fetchPageItems(pageUrl, pageNum)

            if (!pageItems || pageItems.length === 0) {
                console.log(`[D4] No items on page ${pageNum}, stopping`)
                break
            }

            const currentPageIds = pageItems.map(item => item.id)
            const isSameAsPreviousPage =
                previousPageIds &&
                previousPageIds.length === currentPageIds.length &&
                previousPageIds.every((id, index) => id === currentPageIds[index])

            if (isSameAsPreviousPage) {
                console.log(`[D4] Page ${pageNum} is identical to previous page, stopping`)
                break
            }

            previousPageIds = currentPageIds

            // Save page data (like Arc Raiders)
            if (!dryRun) {
                try {
                    await fs.mkdir(pagesDir, { recursive: true })
                    const pageData = {
                        source: pageUrl,
                        fetchedAt: new Date().toISOString(),
                        page: pageNum,
                        itemCount: pageItems.length,
                        data: pageItems,
                    }
                    await fs.writeFile(
                        `${pagesDir}/page-${pageNum}.json`,
                        JSON.stringify(pageData, null, 2),
                        "utf8"
                    )
                } catch (e) {
                    console.log(`[D4] Failed to save page ${pageNum}: ${e.message}`)
                }
            }

            // Extract items and icon URLs
            for (const item of pageItems) {
                if (items.length >= limit) break
                if (seenItemIds.has(item.id)) {
                    duplicateItemsSkipped++
                    continue
                }

                seenItemIds.add(item.id)

                items.push(normalizeRawItem(item))

                if (item.icon && !imageUrls.has(item.icon)) {
                    const url = buildIconUrl(item.icon)
                    imageUrls.set(item.icon, url)
                }
            }

            pagesFetched++
        }

        if (items.length === 0 && !dryRun) {
            const pageFallbackItems = await loadItemsFromSavedPages(output)
            if (pageFallbackItems.length > 0) {
                items.push(...pageFallbackItems)
                for (const item of pageFallbackItems) {
                    seenItemIds.add(item.id)
                    if (item.icon && !imageUrls.has(item.icon)) {
                        imageUrls.set(item.icon, buildIconUrl(item.icon))
                    }
                }
                console.log(`[D4] Fallback: recovered ${items.length} item(s) from saved page files`)
            } else {
                const existingItems = await loadExistingItems(output)
                if (existingItems.length > 0) {
                    items.push(...existingItems)
                    for (const item of existingItems) {
                        seenItemIds.add(item.id)
                        if (item.icon && !imageUrls.has(item.icon)) {
                            imageUrls.set(item.icon, buildIconUrl(item.icon))
                        }
                    }
                    console.log(`[D4] Fallback: reusing existing items.json with ${items.length} item(s)`)
                }
            }
        }

        console.log(`[D4] Fetched ${pagesFetched} page(s) with ${items.length} unique items`)

        // Download images unless dry-run
        let downloadedImages = 0
        let cachedImages = 0
        let missingImages = 0

        if (!dryRun && imageUrls.size > 0) {
            console.log(`[D4] Downloading ${imageUrls.size} unique icons...`)
            const imageDir = path.join(output, "images")
            await fs.mkdir(imageDir, { recursive: true })

            const imageTasks = Array.from(imageUrls.entries()).map(([iconId, url]) => async () =>
                downloadImage(url, imageDir, iconId)
                    .then(result => {
                        if (result.status === "downloaded") downloadedImages++
                        else if (result.status === "cached") cachedImages++
                        else if (result.status === "missing") missingImages++
                    })
                    .catch(err => {
                        console.log(`[D4] Image download error for ${iconId}: ${err.message}`)
                        missingImages++
                    })
            )

            await runWithConcurrency(imageTasks, concurrency)
        }

        if (!dryRun) {
            await fs.mkdir(output, { recursive: true })

            if (items.length > 0) {
                await fs.writeFile(
                    path.join(output, "items.json"),
                    JSON.stringify(items, null, 2),
                    "utf8"
                )
            } else {
                console.log("[D4] No items resolved; skip writing empty items.json")
            }

            const summary = {
                source: "https://www.wowhead.com/diablo-4/items",
                fetchedAt,
                pagesFetched,
                totalItems: items.length,
                duplicateItemsSkipped,
                uniqueIcons: imageUrls.size,
                downloadedImages,
                cachedImages,
                missingImages,
            }
            await fs.writeFile(
                path.join(output, "summary.json"),
                JSON.stringify(summary, null, 2),
                "utf8"
            )

            console.log(`[D4] Written ${items.length} unique items to ${output}/items.json`)
            console.log(`[D4] Duplicates skipped: ${duplicateItemsSkipped}`)
            console.log(`[D4] Images: ${downloadedImages} downloaded, ${cachedImages} cached, ${missingImages} missing`)
        }

        return {
            source: "https://www.wowhead.com/diablo-4/items",
            fetchedAt,
            pagesFetched,
            totalItems: items.length,
            duplicateItemsSkipped,
            uniqueIcons: imageUrls.size,
            downloadedImages,
            cachedImages,
            missingImages,
            args: options,
        }
    } catch (error) {
        console.log(`[D4] Error: ${error.message}`)
        throw error
    }
}

/**
 * Fetch items from a single Wowhead D4 items page
 * Extracts JSON from embedded script tag
 */
async function fetchPageItems(url, pageNum) {
    try {
        const response = await fetch(url)
        if (!response.ok) {
            console.log(`[D4] HTTP ${response.status} for page ${pageNum}`)
            return []
        }

        const html = await response.text()

        const lines = html.split("\n")
        let jsonLine = null

        for (const line of lines) {
            if (line.includes('"id":"items"') && line.includes('"template":"d4-item"') && line.includes('"data":')) {
                jsonLine = line.trim()
                break
            }
        }

        if (!jsonLine) {
            console.log(`[D4] No items script found on page ${pageNum}`)
            return []
        }

        try {
            const arrayStart = jsonLine.indexOf("[{")
            if (arrayStart === -1) {
                console.log(`[D4] Could not find array start on page ${pageNum}`)
                return []
            }

            const jsonStr = jsonLine.substring(arrayStart)
            let parsed = null
            let lastError = null

            try {
                parsed = JSON.parse(jsonStr)
            } catch {
                for (let i = jsonStr.length - 1; i > 0; i--) {
                    try {
                        const attempt = jsonStr.substring(0, i)
                        parsed = JSON.parse(attempt)
                        break
                    } catch (ee) {
                        lastError = ee
                    }
                }
            }

            if (!parsed) {
                console.log(`[D4] Could not parse JSON from page ${pageNum}: ${lastError?.message}`)
                return []
            }

            let items = []
            if (Array.isArray(parsed) && parsed[0]?.data) {
                items = parsed[0].data || []
            } else if (Array.isArray(parsed)) {
                items = parsed
            } else if (parsed.data) {
                items = parsed.data
            }

            console.log(`[D4] Page ${pageNum}: found ${items.length} items`)
            return items
        } catch (e) {
            console.log(`[D4] JSON parse error on page ${pageNum}: ${e.message}`)
            return []
        }
    } catch (error) {
        console.log(`[D4] Failed to fetch page ${pageNum}: ${error.message}`)
        return []
    }
}

/**
 * Build icon URL from Wowhead icon ID
 * Prefix = iconId % 256 (verified from live page assets)
 */
function buildIconUrl(iconId) {
    const prefix = iconId % 256
    return `https://wow.zamimg.com/d4/d4/texture/hash/${prefix}/${iconId}.webp`
}

/**
 * Download a single image and cache it
 */
async function downloadImage(url, outputDir, iconId) {
    const filename = `${iconId}.webp`
    const filepath = path.join(outputDir, filename)

    try {
        await fs.access(filepath)
        return { status: "cached", iconId }
    } catch {
        // File doesn't exist, proceed to download
    }

    try {
        const response = await fetch(url)
        if (!response.ok) {
            return { status: "missing", iconId }
        }

        const buffer = await response.arrayBuffer()
        await fs.writeFile(filepath, Buffer.from(buffer))
        return { status: "downloaded", iconId }
    } catch {
        return { status: "missing", iconId }
    }
}

/**
 * Run an array of promises with concurrency limit
 */
async function runWithConcurrency(tasks, concurrency) {
    if (!tasks.length) return []

    const workerCount = Math.max(1, Math.min(concurrency, tasks.length))
    const results = new Array(tasks.length)
    let cursor = 0

    async function worker() {
        while (cursor < tasks.length) {
            const index = cursor
            cursor++
            results[index] = await tasks[index]()
        }
    }

    await Promise.all(Array.from({ length: workerCount }, () => worker()))
    return results
}
