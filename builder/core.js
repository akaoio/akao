import { write, load, copy, dir, remove, isDirectory, exist } from "../src/core/FS.js"
import { color, icons } from "../src/core/Colors.js"
import { paths } from "./core/config.js"
import { log } from "./core/logger.js"
import { generateRoutes } from "./core/routes.js"
import { processI18n } from "./core/i18n.js"
import { generateHashFiles } from "./core/hash.js"
import { Forex } from "../src/core/Forex.js"
import fs from "fs"

// ============ Helper Functions ============
async function copyAssets(assets) {
    for (const { src, dest, label } of assets) {
        await copy(src, dest)
        log.ok(`Copied ${label}`)
    }
}

async function processYamlDirectory(srcPath, destPath, { recursive = false, filter = null } = {}) {
    const files = await dir(srcPath)
    const filtered = filter ? files.filter(filter) : files
    let processed = 0

    for (const file of filtered) {
        const fullSrcPath = [...srcPath, file]

        if (recursive) {
            const subFiles = await dir(fullSrcPath)
            for (const subFile of subFiles) {
                const fullSubPath = [...fullSrcPath, subFile]

                // Check if it's a directory - if so, copy it entirely
                if (await isDirectory(fullSubPath)) {
                    await copy(fullSubPath, [...destPath, file, subFile])
                    continue
                }

                // Otherwise, try to load and convert YAML/JSON
                const data = await load(fullSubPath)
                if (data) {
                    const jsonName = subFile.replace(/\.(yaml|yml)$/, ".json")
                    await write([...destPath, file, jsonName], data)
                    processed++
                } else await copy(fullSubPath, [...destPath, file, subFile])
            }
        } else {
            if (await isDirectory(fullSrcPath)) {
                await copy(fullSrcPath, [...destPath, file])
                continue
            }

            const data = await load(fullSrcPath)
            if (data) {
                const jsonName = file.replace(/\.(yaml|yml)$/, ".json")
                await write([...destPath, jsonName], data)
                processed++
            }
        }
    }

    return { total: filtered.length, processed }
}

function normalizeTag(tag = "") {
    return String(tag || "")
        .toLowerCase()
        .replace(/[^a-z0-9]+/g, "-")
        .replace(/^-+|-+$/g, "")
}

function normalizeTags(tags = []) {
    return [...new Set((tags || []).map(normalizeTag).filter(Boolean))].sort()
}

// ============ Main Build Process ============
log.start("Starting build process...")

// Check forex rates
const forexPath = [...paths.src.statics, "forex.yaml"].join("/")
let shouldUpdateForex = true

try {
    if (fs.existsSync(forexPath)) {
        const stats = fs.statSync(forexPath)
        const hoursSinceUpdate = (Date.now() - stats.mtimeMs) / (1000 * 60 * 60)

        if (hoursSinceUpdate < 24) {
            shouldUpdateForex = false
            log.ok(`Found cached forex rates (${Math.round(hoursSinceUpdate)} hours old)`)
        }
    }
} catch (e) {
    // File doesn't exist or error checking, will fetch new data
}

// Clean build folder, preserving build/geo and crypto outputs if they exist
log.info("Cleaning build folder...")
const geoPath = [paths.build.root, "geo"]
const hasGeo = await exist(geoPath)

// Crypto-generated directories inside build/statics
const cryptoDirs = ["ABIs", "chains"]
const preservedStaticsDirs = [...cryptoDirs, "items"]
const hasCryptoDirs = (await Promise.all(cryptoDirs.map((d) => exist([...paths.build.statics, d])))).some(Boolean)
const hasItemsDir = await exist([...paths.build.statics, "items"])
const hasCryptoImages = await exist([...paths.build.root, "images", "cryptos"])

if (hasGeo || hasCryptoDirs || hasItemsDir) {
    const preserved = []
    if (hasGeo) preserved.push("geo")
    if (hasCryptoDirs) preserved.push("statics (crypto data)")
    if (hasItemsDir) preserved.push("statics/items")
    if (hasCryptoImages) preserved.push("images/cryptos")
    log.info(`Preserving: ${preserved.join(", ")}, cleaning other build files...`)

    const buildItems = await dir([paths.build.root])
    for (const item of buildItems) {
        if (item === "geo" && hasGeo) continue

        if (item === "statics" && (hasCryptoDirs || hasItemsDir)) {
            // Clean statics selectively, preserving configured subdirs
            const staticsItems = await dir(paths.build.statics)
            for (const staticsItem of staticsItems) {
                if (preservedStaticsDirs.includes(staticsItem)) continue
                await remove([...paths.build.statics, staticsItem])
            }
            continue
        }

        if (item === "images" && hasCryptoImages) {
            // Clean images selectively, preserving cryptos subfolder
            const imageItems = await dir([...paths.build.root, "images"])
            for (const imageItem of imageItems) {
                if (imageItem === "cryptos") continue
                await remove([...paths.build.root, "images", imageItem])
            }
            continue
        }

        await remove([paths.build.root, item])
    }
} else await remove([paths.build.root])

log.ok("Cleaned build folder")

// Load configuration
log.info("Loading configuration and data...")
const localesConfig = await load([...paths.src.statics, "locales.yaml"])
const locales = localesConfig.map((locale) => locale.code)
const system = (await load([...paths.src.statics, "system.yaml"])) || { pagination: 10 }

// Load items metadata — distinguish core-managed (flat) from game-derived (nested)
const itemDirs = await dir(paths.src.items)
const coreItems = []          // flat item-ids under src/statics/items/<item-id>/
const gameItemsMap = {}       // { gameId: [item-id, ...] }
const allTags = new Set()

for (const name of itemDirs) {
    const rootMetaPath = [...paths.src.items, name, "meta.yaml"]
    const meta = (await exist(rootMetaPath)) ? await load(rootMetaPath) : null
    if (meta) {
        // Has meta.yaml directly → core-managed item
        coreItems.push(name)
        normalizeTags(meta.tags).forEach((tag) => allTags.add(tag))
    } else {
        // No meta.yaml → check if it's a game namespace containing item subdirs
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

const totalItemCount = coreItems.length + Object.values(gameItemsMap).reduce((s, a) => s + a.length, 0)
log.ok(`Loaded: ${locales.length} locales, ${coreItems.length} core items, ${Object.keys(gameItemsMap).length} game namespaces (${totalItemCount} total), ${allTags.size} unique tags`)

// Load games metadata
const gameDirs = await dir(paths.src.games)
const games = []
for (const name of gameDirs) {
    const meta = await load([...paths.src.games, name, "meta.yaml"])
    if (meta) games.push(name)
}
log.ok(`Loaded: ${games.length} games`)

// Fetch forex rates
log.info("Checking forex rates...")
const forex = new Forex()
await forex.init()

if (shouldUpdateForex) {
    await forex.update()
    await write([...paths.src.statics, "forex.yaml"], forex.rates)
    log.ok("Fetched and saved new forex rates")
} else log.ok("Using cached forex rates")

// Build static files (YAML → JSON)
log.info("Building static files...")
const { processed: staticCount } = await processYamlDirectory(paths.src.statics, paths.build.statics, { filter: (file) => file.endsWith(".yaml") || file.endsWith(".yml") || file.endsWith(".json") })
log.ok(`Built ${staticCount} static files`)

// Build domains mapping
log.info("Building domains mapping...")
const domainsData = await load([...paths.src.statics, "domains.yaml"])
let domainCount = 0
for (const [domain, value] of Object.entries(domainsData)) {
    await write([...paths.build.statics, "domains", `${domain}.json`], { site: value })
    domainCount++
}
log.ok(`Built ${domainCount} domain mappings`)

// Build core-managed items (YAML → JSON, flat structure only)
log.info("Building core-managed items (YAML → JSON)...")
for (const itemId of coreItems) {
    await processYamlDirectory(
        [...paths.src.items, itemId],
        [...paths.build.statics, "items", itemId],
        { recursive: false }
    )
}
for (const itemId of coreItems) {
    const metaPath = [...paths.build.statics, "items", itemId, "meta.json"]
    const meta = await load(metaPath)
    if (meta?.tags) await write(metaPath, { ...meta, tags: normalizeTags(meta.tags) })
}
log.ok(`Built ${coreItems.length} core-managed items`)

// Game-derived item JSON output in build/statics/items/<game-id>/<item-id>/ is owned by build:games.
// build:core only consumes gameItemsMap for indexes/pagination/routes and does not re-emit this subtree.
log.info("Skipping game-derived item emission in build:core (owned by build:games)")

// Generate items pagination
log.info("Generating items pagination...")
const pagination = system.pagination

const allItemKeys = [
    ...coreItems,
    ...Object.entries(gameItemsMap).flatMap(([gameId, itemIds]) => itemIds.map((itemId) => `${gameId}/${itemId}`))
].sort((a, b) => a.localeCompare(b))

// Clean legacy/previous pagination outputs while preserving item directories
const itemsRoot = [...paths.build.statics, "items"]
if (await exist(itemsRoot)) {
    const entries = await dir(itemsRoot)
    for (const entry of entries) {
        if (entry === "all") {
            await remove([...itemsRoot, entry])
            continue
        }
        if (entry === "meta.json" || /^\d+\.json$/.test(entry)) {
            await remove([...itemsRoot, entry])
        }
    }
}

const totalAllItemPages = Math.ceil(allItemKeys.length / pagination)
await write([...paths.build.statics, "items", "meta.json"], {
    children: allItemKeys.length,
    pages: Math.max(1, totalAllItemPages)
})
for (let page = 1; page <= Math.max(1, totalAllItemPages); page++) {
    const start = (page - 1) * pagination
    const pageItems = allItemKeys.slice(start, start + pagination)
    await write([...paths.build.statics, "items", `${page}.json`], pageItems)
}
log.ok(`Generated ${Math.max(1, totalAllItemPages)} item page(s) (${allItemKeys.length} items)`)

log.info("Skipping tags build in core (moved to build:index)")

// Build games
log.info("Building games (YAML → JSON)...")
await processYamlDirectory(paths.src.games, [...paths.build.statics, "games"], { recursive: true })
log.ok(`Built ${games.length} games`)

// Generate games pagination
log.info("Generating games pagination...")
const totalGamePages = Math.ceil(games.length / pagination)
await write([...paths.build.statics, "games", "meta.json"], {
    children: games.length,
    pages: totalGamePages
})
for (let page = 1; page <= totalGamePages; page++) {
    const start = (page - 1) * pagination
    const pageGames = games.slice(start, start + pagination)
    await write([...paths.build.statics, "games", `${page}.json`], pageGames)
}
log.ok(`Generated ${totalGamePages} game pages`)

// Generate per-game item pagination (from discovered gameItemsMap, owned by build:core)
log.info("Generating per-game item pagination...")
for (const [gameId, itemIds] of Object.entries(gameItemsMap)) {
    const totalGameItemPages = Math.ceil(itemIds.length / pagination)
    await write([...paths.build.statics, "games", gameId, "items", "meta.json"], {
        children: itemIds.length,
        pages: Math.max(1, totalGameItemPages)
    })
    for (let page = 1; page <= Math.max(1, totalGameItemPages); page++) {
        const start = (page - 1) * pagination
        const pageItems = itemIds.slice(start, start + pagination)
        await write([...paths.build.statics, "games", gameId, "items", `${page}.json`], pageItems)
    }
}
log.ok(`Generated per-game item pagination for ${Object.keys(gameItemsMap).length} game namespace(s)`)

// Build sites
log.info("Building sites (YAML → JSON)...")
const siteDirs = await dir(paths.src.sites)
await processYamlDirectory(paths.src.sites, [...paths.build.statics, "sites"], { recursive: true })
log.ok(`Built ${siteDirs.length} sites`)

// Copy assets
await copyAssets([
    { src: paths.src.core, dest: paths.build.core, label: "core folder" },
    { src: paths.src.UI, dest: paths.build.UI, label: "UI folder" },
    { src: ["src", "test.html"], dest: [...paths.build.root, "test.html"], label: "test.html" },
    { src: paths.src.importmap, dest: [...paths.build.root, "importmap.json"], label: "importmap.json" },
    { src: ["node_modules", "bootstrap-icons", "icons"], dest: [...paths.build.root, "images", "icons"], label: "bootstrap icons" },
    { src: ["node_modules", "ethers", "dist", "ethers.min.js"], dest: [...paths.build.core, "Ethers.js"], label: "ethers" }
])

// Prepare ggwave under build/core/Wave/ for worker imports
const ggwaveCode = fs.readFileSync("node_modules/ggwave/ggwave.js", "utf8")
await write([...paths.build.core, "Wave", "ggwave.js"], `${ggwaveCode}\n\nexport default ggwave_factory\n`)
log.ok("Prepared ggwave → build/core/Wave/ggwave.js")

// Copy uqr ESM library
log.info("Copying uqr to build...")
await copy(["node_modules", "uqr", "dist", "index.mjs"], [...paths.build.core, "QR", "encoder.js"])
log.ok("Copied uqr → build/core/QR/encoder.js")

// Copy qr-scanner ESM library
log.info("Copying qr-scanner to build...")
await copy(["node_modules", "qr-scanner", "qr-scanner.min.js"], [...paths.build.core, "QR", "decoder.js"])
log.ok("Copied qr-scanner → build/core/QR/decoder.js")
await copy(["node_modules", "qr-scanner", "qr-scanner-worker.min.js"], [...paths.build.core, "QR", "qr-scanner-worker.min.js"])
log.ok("Copied qr-scanner worker → build/core/QR/qr-scanner-worker.min.js")

// Copy gun library files to GDB folder
log.info("Copying gun library to GDB...")
const gunFiles = ["gun.js", "sea.js", ["lib", "radix.js"], ["lib", "radisk.js"], ["lib", "rindexed.js"], ["lib", "store.js"]]
for (const filePath of gunFiles) {
    const src = Array.isArray(filePath) ? ["node_modules", "@akaoio", "gun", ...filePath] : ["node_modules", "@akaoio", "gun", filePath]
    const dest = [...paths.build.core, "GDB", Array.isArray(filePath) ? filePath[filePath.length - 1] : filePath]
    await copy(src, dest)
}
log.ok(`Copied gun files to GDB`)

// Build routes list using regex pattern and post-process
log.info("Building routes list...")
const found = await dir(paths.src.routes, /index\.js$/)
// Keep only directories that have index.js by stripping the suffix
// Sort: fixed-first-segment routes before fully-dynamic ones to ensure correct Router matching order
const routeDirs = Array.from(new Set(found.filter((p) => p.endsWith("index.js")).map((p) => p.replace(/\/index\.js$/, ""))))
    .filter((route) => route !== "tag/[tag]")
    .sort((a, b) => {
    const aFirstDynamic = a.split("/")[0]?.startsWith("[")
    const bFirstDynamic = b.split("/")[0]?.startsWith("[")
    if (aFirstDynamic && !bFirstDynamic) return 1
    if (!aFirstDynamic && bFirstDynamic) return -1
    return a.localeCompare(b)
})
await write([...paths.build.statics, "routes.json"], routeDirs)
log.ok(`Built routes list with ${routeDirs.length} routes`)

// Process i18n
log.info("Processing i18n files...")
const localeCount = await processI18n(locales)
log.ok(`Created ${localeCount} locale files`)

// Generate routes
log.info("Generating routes...")
const indexContent = await load(paths.src.index)
const routeCount = await generateRoutes(locales, coreItems, [], games, indexContent, "build", routeDirs, gameItemsMap)
log.ok(`Created ${routeCount} route files`)

// Note: geo data is built separately via npm run geo:build to build/geo/

// Note: geo metadata (countries, features) is built by geo:build to build/geo/

// Generate hash files for all JSON files in build directory (excluding geo)
log.info("Generating hash files...")
const hashResult = await generateHashFiles(paths.build.root, ["geo"])
log.ok(`Created ${hashResult.hashFiles} hash files`)

// Summary
log.section("========================================")
console.log(`${icons.done} ${color.ok("Locales")}: ${locales.length}`)
console.log(`${icons.done} ${color.ok("Core Items")}: ${coreItems.length}`)
console.log(`${icons.done} ${color.ok("Game Namespaces")}: ${Object.keys(gameItemsMap).length} (${totalItemCount} items)`)
console.log(`${icons.done} ${color.ok("Items (Total)")}: ${totalItemCount}`)
console.log(`${icons.done} ${color.ok("Unique Tags")}: ${allTags.size}`)
console.log(`${icons.done} ${color.ok("Routes Created")}: ${routeCount}`)
console.log(`${icons.done} ${color.ok("Gun Files")}: ${gunFiles.length}`)
console.log(`${icons.done} ${color.ok("Hash Files")}: ${hashResult.hashFiles}`)
if (await exist(geoPath)) console.log(`${icons.done} ${color.ok("Geo Data")}: ✓ cached`)

log.section("========================================")
log.start("Build completed successfully!")
