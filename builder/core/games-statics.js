import { write, load, exist } from "../../src/core/FS.js"
import { paths } from "./config.js"
import { log } from "./logger.js"

const CATALOG_PAGE_SIZE = 36

async function buildGameItemFiles(gameId, rawItems, detailMapper, localeCodes) {
    for (const raw of rawItems) {
        const { meta: gameMeta, locale: localeData } = detailMapper(raw)
        const itemDir = [...paths.build.statics, "items", raw.id]

        const existingMeta = (await exist([...itemDir, "meta.json"])) ? await load([...itemDir, "meta.json"]) : null
        await write([...itemDir, "meta.json"], { ...gameMeta, ...(existingMeta || {}) })

        await Promise.all(
            localeCodes.map(async (code) => {
                const localePath = [...itemDir, `${code}.json`]
                if (!(await exist(localePath))) await write(localePath, localeData)
            })
        )
    }
    log.ok(`  ${gameId}: wrote ${rawItems.length} item detail files`)
}

async function buildGameCatalog(gameId, catalogMapper, detailMapper) {
    if (!catalogMapper) {
        log.info(`  No catalogMapper for "${gameId}", skipping.`)
        return
    }

    const rawItemsPath = ["games", gameId, "items.json"]
    if (!(await exist(rawItemsPath))) {
        log.info(`  No raw items found at games/${gameId}/items.json, skipping.`)
        return
    }

    const rawItems = await load(rawItemsPath)
    if (!Array.isArray(rawItems)) return

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
    const catalogDir = [...paths.build.statics, "games", gameId, "catalog"]

    await write([...catalogDir, "meta.json"], {
        children: items.length,
        pages: totalPages,
        types,
        rarities,
        ...(subtypes && { subtypes }),
        ...(classes && { classes })
    })

    await Promise.all(
        Array.from({ length: totalPages }, (_, i) => {
            const page = i + 1
            const start = (page - 1) * CATALOG_PAGE_SIZE
            return write([...catalogDir, `${page}.json`], items.slice(start, start + CATALOG_PAGE_SIZE))
        })
    )

    log.ok(`  ${gameId}: ${items.length} items → ${totalPages} catalog pages`)

    if (detailMapper) {
        const localesConfig = await load([...paths.src.statics, "locales.yaml"])
        const localeCodes = localesConfig.map((l) => l.code)
        await buildGameItemFiles(gameId, rawItems, detailMapper, localeCodes)
    }
}

/**
 * @param {string[]} gameIds - game IDs from src/statics/games/
 * @param {Array<{id: string, catalogMapper?: Function, detailMapper?: Function}>} gameRegistry - from builder/games/index.js
 */
export async function processGamesCatalog(gameIds, gameRegistry = []) {
    log.info("Building game catalogs...")

    const registryMap = new Map(gameRegistry.map((g) => [g.id, g]))

    for (const gameId of gameIds) {
        const game = registryMap.get(gameId)
        await buildGameCatalog(gameId, game?.catalogMapper ?? null, game?.detailMapper ?? null)
    }

    log.ok(`Generated game catalogs for ${gameIds.length} game(s)`)
}
