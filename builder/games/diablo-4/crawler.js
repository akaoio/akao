import fs from "node:fs/promises"
import path from "node:path"

async function ensureDirectories(root) {
    await fs.mkdir(path.join(root, "pages"), { recursive: true })
    await fs.mkdir(path.join(root, "images"), { recursive: true })
}

export async function crawlDiablo4Items(options = {}) {
    const output = options.output ? path.resolve(options.output) : path.resolve("games", "diablo-4")
    await ensureDirectories(output)

    const summary = {
        fetchedAt: new Date().toISOString(),
        pagesFetched: 0,
        totalItems: 0,
        downloadedImages: 0,
        cachedImages: 0,
        missingImages: 0,
        message: "Crawler diablo-4 is scaffolded and ready for implementation",
        args: {
            dryRun: !!options.dryRun,
            output,
        },
    }

    await fs.writeFile(path.join(output, "items.json"), JSON.stringify([], null, 2), "utf8")
    await fs.writeFile(path.join(output, "summary.json"), JSON.stringify(summary, null, 2), "utf8")

    return summary
}
