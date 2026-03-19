import fs from "node:fs/promises"
import path from "node:path"
import vm from "node:vm"

const BASE_URL = "https://metaforge.app/arc-raiders/database/items/page"
const DEFAULT_MAX_PAGES = 14
const DEFAULT_LIMIT = 40
const DEFAULT_CONCURRENCY = 8

function extractBalancedObject(source, marker) {
    const markerIndex = source.indexOf(marker)
    if (markerIndex < 0) {
        throw new Error(`Marker not found: ${marker}`)
    }

    const start = markerIndex + marker.length
    let depth = 1
    let inString = false
    let quote = ""
    let escaped = false

    for (let i = start; i < source.length; i += 1) {
        const char = source[i]

        if (inString) {
            if (escaped) {
                escaped = false
                continue
            }

            if (char === "\\") {
                escaped = true
                continue
            }

            if (char === quote) {
                inString = false
                quote = ""
            }

            continue
        }

        if (char === '"' || char === "'" || char === "`") {
            inString = true
            quote = char
            continue
        }

        if (char === "{") {
            depth += 1
            continue
        }

        if (char === "}") {
            depth -= 1
            if (depth === 0) {
                return source.slice(start, i)
            }
        }
    }

    throw new Error("Failed to extract balanced object")
}

function parsePageDataObject(html) {
    const objectBody = extractBalancedObject(html, "data:{tab:\"items\"")
    const literal = `{tab:\"items\"${objectBody}}`
    const parsed = vm.runInNewContext(`(${literal})`, Object.create(null), {
        timeout: 1000,
    })

    if (!parsed || !Array.isArray(parsed.data)) {
        throw new Error("Parsed item payload is invalid")
    }

    return parsed
}

async function fetchItemsPage(page, limit) {
    const url = `${BASE_URL}/${page}?limit=${limit}`
    const res = await fetch(url, {
        headers: {
            "user-agent": "shop-games-builder/1.0",
        },
    })

    if (!res.ok) {
        throw new Error(`Failed to fetch page ${page}: ${res.status} ${res.statusText}`)
    }

    const html = await res.text()
    const data = parsePageDataObject(html)
    return {
        url,
        page,
        payload: data,
    }
}

function extFromUrl(url) {
    try {
        const pathname = new URL(url).pathname
        const ext = path.extname(pathname)
        return ext || ".webp"
    } catch {
        return ".webp"
    }
}

async function ensureDirectories(root) {
    await fs.mkdir(path.join(root, "pages"), { recursive: true })
    await fs.mkdir(path.join(root, "images"), { recursive: true })
}

async function fileExists(filePath) {
    try {
        await fs.access(filePath)
        return true
    } catch {
        return false
    }
}

async function downloadImage(item, imageDir) {
    const { id, icon } = item
    if (!icon) {
        return { id, status: "no-icon" }
    }

    const ext = extFromUrl(icon)
    const outputPath = path.join(imageDir, `${id}${ext}`)
    if (await fileExists(outputPath)) {
        return { id, status: "cached", file: outputPath }
    }

    const res = await fetch(icon, {
        headers: {
            "user-agent": "shop-games-builder/1.0",
            referer: "https://metaforge.app/",
        },
    })

    if (!res.ok) {
        throw new Error(`Failed image ${id}: ${res.status} ${res.statusText}`)
    }

    const bytes = Buffer.from(await res.arrayBuffer())
    await fs.writeFile(outputPath, bytes)

    return { id, status: "downloaded", file: outputPath }
}

async function runWithConcurrency(items, concurrency, worker) {
    const queue = [...items]
    const results = []

    const runners = Array.from({ length: Math.min(concurrency, queue.length) }, async () => {
        while (queue.length) {
            const item = queue.shift()
            if (!item) {
                continue
            }
            const result = await worker(item)
            results.push(result)
        }
    })

    await Promise.all(runners)
    return results
}

function normalizeOptions(options = {}) {
    return {
        maxPages: Number.isFinite(options.maxPages) && options.maxPages > 0 ? options.maxPages : DEFAULT_MAX_PAGES,
        concurrency:
            Number.isFinite(options.concurrency) && options.concurrency > 0
                ? options.concurrency
                : DEFAULT_CONCURRENCY,
        limit: Number.isFinite(options.limit) && options.limit > 0 ? options.limit : DEFAULT_LIMIT,
        output: options.output ? path.resolve(options.output) : path.resolve("games", "arc-raiders"),
        dryRun: !!options.dryRun,
    }
}

export async function crawlArcRaidersItems(options = {}) {
    const args = normalizeOptions(options)
    await ensureDirectories(args.output)

    const pagePayloads = []

    for (let page = 1; page <= args.maxPages; page += 1) {
        const response = await fetchItemsPage(page, args.limit)
        const payload = response.payload
        pagePayloads.push(payload)

        const pageFile = path.join(args.output, "pages", `page-${page}.json`)
        await fs.writeFile(
            pageFile,
            JSON.stringify(
                {
                    source: response.url,
                    fetchedAt: new Date().toISOString(),
                    page,
                    limit: payload.limit,
                    count: payload.count,
                    itemCount: payload.data.length,
                    data: payload.data,
                },
                null,
                2,
            ),
            "utf8",
        )

        const totalPagesByCount = payload.count
            ? Math.ceil(payload.count / (payload.limit || args.limit || DEFAULT_LIMIT))
            : args.maxPages

        if (page === 1 && totalPagesByCount < args.maxPages) {
            args.maxPages = totalPagesByCount
        }
    }

    const byId = new Map()
    for (const payload of pagePayloads) {
        for (const item of payload.data) {
            byId.set(item.id, item)
        }
    }

    const items = [...byId.values()].sort((a, b) => a.id.localeCompare(b.id))
    const imagesDir = path.join(args.output, "images")

    let imageResults = []
    if (!args.dryRun) {
        imageResults = await runWithConcurrency(items, args.concurrency, (item) =>
            downloadImage(item, imagesDir),
        )
    }

    const summary = {
        source: BASE_URL,
        fetchedAt: new Date().toISOString(),
        pagesFetched: pagePayloads.length,
        totalItems: items.length,
        downloadedImages: imageResults.filter((r) => r.status === "downloaded").length,
        cachedImages: imageResults.filter((r) => r.status === "cached").length,
        missingImages: imageResults.filter((r) => r.status === "no-icon").length,
        args,
    }

    await fs.writeFile(path.join(args.output, "items.json"), JSON.stringify(items, null, 2), "utf8")
    await fs.writeFile(path.join(args.output, "summary.json"), JSON.stringify(summary, null, 2), "utf8")

    return summary
}
