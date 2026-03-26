import { load, write, dir, isDirectory, exist } from "../src/core/FS.js"
import { paths } from "./core/config.js"
import { log } from "./core/logger.js"
import { generateRoutes } from "./core/routes.js"

function normalizeTag(tag = "") {
    return String(tag || "")
        .toLowerCase()
        .replace(/[^a-z0-9]+/g, "-")
        .replace(/^-+|-+$/g, "")
}

function normalizeTags(tags = []) {
    return [...new Set((tags || []).map(normalizeTag).filter(Boolean))].sort()
}

log.start("Starting index build (tags + tag routes)...")

const localesConfig = await load([...paths.src.statics, "locales.yaml"])
const locales = localesConfig.map((locale) => locale.code)
const system = (await load([...paths.src.statics, "system.yaml"])) || { pagination: 10 }

const itemDirs = await dir(paths.src.items)
const coreItems = []
const gameItemsMap = {}
const allTags = new Set()

for (const name of itemDirs) {
    const rootMetaPath = [...paths.src.items, name, "meta.yaml"]
    const meta = (await exist(rootMetaPath)) ? await load(rootMetaPath) : null
    if (meta) {
        coreItems.push(name)
        normalizeTags(meta.tags).forEach((tag) => allTags.add(tag))
    } else {
        const subDirs = await dir([...paths.src.items, name])
        const nested = []
        for (const sub of subDirs) {
            if (await isDirectory([...paths.src.items, name, sub])) {
                const subMeta = await load([...paths.src.items, name, sub, "meta.yaml"])
                if (subMeta) {
                    nested.push(sub)
                    normalizeTags(subMeta.tags).forEach((tag) => allTags.add(tag))
                }
            }
        }
        if (nested.length > 0) gameItemsMap[name] = nested
    }
}

const pagination = system.pagination
const tagsList = Array.from(allTags).sort()

log.info("Building tags pagination...")
const totalTagPages = Math.ceil(tagsList.length / pagination)
await write([...paths.build.statics, "tags", "meta.json"], {
    children: tagsList.length,
    pages: Math.max(1, totalTagPages)
})
for (let page = 1; page <= Math.max(1, totalTagPages); page++) {
    const start = (page - 1) * pagination
    const pageTags = tagsList.slice(start, start + pagination)
    await write([...paths.build.statics, "tags", `${page}.json`], pageTags)
}

log.info("Building per-tag item pages...")
for (const tag of tagsList) {
    const tagItems = []

    for (const itemName of coreItems) {
        const meta = await load([...paths.build.statics, "items", itemName, "meta.json"])
        if (normalizeTags(meta?.tags).includes(tag)) tagItems.push(itemName)
    }

    for (const [gameId, itemIds] of Object.entries(gameItemsMap)) {
        for (const itemId of itemIds) {
            const meta = await load([...paths.src.items, gameId, itemId, "meta.yaml"])
            if (normalizeTags(meta?.tags).includes(tag)) tagItems.push(`${gameId}/${itemId}`)
        }
    }

    const tagPages = Math.ceil(tagItems.length / pagination)
    await write([...paths.build.statics, "tags", tag, "meta.json"], {
        children: tagItems.length,
        pages: Math.max(1, tagPages)
    })

    for (let page = 1; page <= Math.max(1, tagPages); page++) {
        const start = (page - 1) * pagination
        const pageItems = tagItems.slice(start, start + pagination)
        await write([...paths.build.statics, "tags", tag, `${page}.json`], pageItems)
    }
}

log.ok(`Built tags index: ${tagsList.length} tags`)

const gamesDirs = await dir(paths.src.games)
const games = []
for (const name of gamesDirs) {
    const meta = await load([...paths.src.games, name, "meta.yaml"])
    if (meta) games.push(name)
}

const indexContent = await load(paths.src.index)
log.info("Generating tag routes...")
const tagRouteCount = await generateRoutes(locales, coreItems, tagsList, games, indexContent, "build", ["tag/[tag]"], gameItemsMap)
log.ok(`Created ${tagRouteCount} tag-route files`) 

log.start("Index build completed successfully!")
