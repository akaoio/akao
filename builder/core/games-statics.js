import fs from "node:fs/promises"
import path from "node:path"
import { log } from "./logger.js"

const CATALOG_PAGE_SIZE = 36

async function readJsonFile(filePath) {
    try {
        const raw = await fs.readFile(filePath, "utf8")
        return JSON.parse(raw)
    } catch {
        return null
    }
}

async function writeJsonFile(filePath, data) {
    await fs.mkdir(path.dirname(filePath), { recursive: true })
    await fs.writeFile(filePath, JSON.stringify(data), "utf8")
}

async function buildGameCatalog(gameId, catalogMapper, buildStaticsRoot) {
    if (!catalogMapper) {
        log.info(`  No catalogMapper for "${gameId}", skipping.`)
        return
    }

    const rawPath = path.resolve("games", gameId, "items.json")
    const rawItems = await readJsonFile(rawPath)
    if (!rawItems || !Array.isArray(rawItems)) {
        log.info(`  No raw items found at ${rawPath}, skipping.`)
        return
    }

    const items = rawItems.map(catalogMapper).sort((a, b) => (a.name || "").localeCompare(b.name || ""))

    // Collect unique types and rarities for filter UI
    const types = [...new Set(items.map((i) => i.type).filter(Boolean))].sort()
    const rarities = [...new Set(items.map((i) => i.rarity).filter(Boolean))].sort()

    // Collect subtypes if present (e.g. Arc Raiders weapon subcategories)
    const rawSubtypes = items.map((i) => i.subtype).filter(Boolean)
    const subtypes = rawSubtypes.length ? [...new Set(rawSubtypes)].sort() : undefined

    // Collect classes if present (e.g. Diablo 4 player classes)
    const rawClasses = items.flatMap((i) => i.classes || []).filter(Boolean)
    const classes = rawClasses.length ? [...new Set(rawClasses)].sort() : undefined

    const totalPages = Math.ceil(items.length / CATALOG_PAGE_SIZE)
    const catalogDir = path.join(buildStaticsRoot, "games", gameId, "catalog")

    await writeJsonFile(path.join(catalogDir, "meta.json"), {
        children: items.length,
        pages: totalPages,
        types,
        rarities,
        ...(subtypes && { subtypes }),
        ...(classes && { classes })
    })

    for (let page = 1; page <= totalPages; page++) {
        const start = (page - 1) * CATALOG_PAGE_SIZE
        const pageItems = items.slice(start, start + CATALOG_PAGE_SIZE)
        await writeJsonFile(path.join(catalogDir, `${page}.json`), pageItems)
    }

    log.ok(`  ${gameId}: ${items.length} items → ${totalPages} catalog pages`)
}

/**
 * @param {string[]} gameIds - game IDs from src/statics/games/
 * @param {Array<{id: string, catalogMapper?: Function}>} gameRegistry - from builder/games/index.js
 * @param {string} buildStaticsRoot
 */
export async function processGamesCatalog(gameIds, gameRegistry = [], buildStaticsRoot = path.resolve("build", "statics")) {
    log.info("Building game catalogs...")

    const registryMap = new Map(gameRegistry.map((g) => [g.id, g]))

    for (const gameId of gameIds) {
        const game = registryMap.get(gameId)
        await buildGameCatalog(gameId, game?.catalogMapper ?? null, buildStaticsRoot)
    }

    log.ok(`Generated game catalogs for ${gameIds.length} game(s)`)
}
