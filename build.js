import { write, load, copy, dir, remove, isDirectory, exist } from "./src/core/FS.js"
import { color, icons } from "./src/core/Colors.js"
import { paths } from "./src/core/Build/config.js"
import { log } from "./src/core/Build/logger.js"
import { generateRoutes } from "./src/core/Build/routes.js"
import { processI18n } from "./src/core/Build/i18n.js"
import { generateHashFiles } from "./src/core/Build/hash.js"
import { Forex } from "./src/core/Forex.js"
import fs from "fs"
import path from "path"

// ============ Helper Functions ============
async function copyGeoWithProgress(srcPath, destPath) {
    let fileCount = 0
    let lastLog = Date.now()
    
    function copyRecursive(src, dest) {
        if (!fs.existsSync(src)) return
        
        const stats = fs.statSync(src)
        
        if (stats.isFile()) {
            const destDir = path.dirname(dest)
            if (!fs.existsSync(destDir)) {
                fs.mkdirSync(destDir, { recursive: true })
            }
            fs.copyFileSync(src, dest)
            fileCount++
            
            // Log progress every 10k files or every 5 seconds
            const now = Date.now()
            if (fileCount % 10000 === 0 || now - lastLog > 5000) {
                console.log(`  Copied ${fileCount.toLocaleString()} files...`)
                lastLog = now
            }
        } else if (stats.isDirectory()) {
            if (!fs.existsSync(dest)) {
                fs.mkdirSync(dest, { recursive: true })
            }
            
            const entries = fs.readdirSync(src)
            for (const entry of entries) {
                copyRecursive(path.join(src, entry), path.join(dest, entry))
            }
        }
    }
    
    copyRecursive(srcPath, destPath)
    return fileCount
}

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
                    const jsonName = subFile.replace(/\.(yaml|yml)$/, '.json')
                    await write([...destPath, file, jsonName], data)
                    processed++
                } else {
                    await copy(fullSubPath, [...destPath, file, subFile])
                }
            }
        } else {
            const data = await load(fullSrcPath)
            if (data) {
                const jsonName = file.replace(/\.(yaml|yml)$/, '.json')
                await write([...destPath, jsonName], data)
                processed++
            }
        }
    }

    return { total: filtered.length, processed }
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

// Clean
log.info("Cleaning build folder...")
// Preserve build/statics/geo if it exists by removing everything except geo
const geoPath = [...paths.build.statics, "geo"]
const hasGeo = await exist(geoPath)

if (hasGeo) {
    // Selectively remove build contents while preserving geo
    log.info("Preserving geo data, cleaning other build files...")
    const buildItems = await dir([paths.build.root])
    for (const item of buildItems) {
        const itemPath = [paths.build.root, item]
        if (item === "statics") {
            // Clean statics except geo
            const staticsItems = await dir([...paths.build.root, "statics"])
            for (const staticsItem of staticsItems) {
                if (staticsItem !== "geo") {
                    await remove([...paths.build.root, "statics", staticsItem])
                }
            }
        } else {
            await remove(itemPath)
        }
    }
} else {
    await remove([paths.build.root])
}
log.ok("Cleaned build folder")

// Load configuration
log.info("Loading configuration and data...")
const localesConfig = await load([...paths.src.statics, "locales.yaml"])
const locales = localesConfig.map(locale => locale.code)
const system = await load([...paths.src.statics, "system.yaml"]) || { pagination: 10 }

// Load items metadata
const itemDirs = await dir(paths.src.items)
const items = []
const allTags = new Set()

for (const name of itemDirs) {
    const meta = await load([...paths.src.items, name, 'meta.yaml'])
    if (meta) {
        items.push(name)
        meta.tags?.forEach(tag => allTags.add(tag))
    }
}

log.ok(`Loaded: ${locales.length} locales, ${items.length} items, ${allTags.size} unique tags`)

// Fetch forex rates
log.info("Checking forex rates...")
const forex = new Forex()
await forex.init()

if (shouldUpdateForex) {
    await forex.update()
    await write([...paths.src.statics, "forex.yaml"], forex.rates)
    log.ok("Fetched and saved new forex rates")
} else {
    log.ok("Using cached forex rates")
}

// Build static files (YAML → JSON)
log.info("Building static files...")
const { processed: staticCount } = await processYamlDirectory(
    paths.src.statics,
    paths.build.statics,
    { filter: file => file.endsWith('.yaml') || file.endsWith('.yml') || file.endsWith('.json') }
)
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

// Build items
log.info("Building items (YAML → JSON)...")
await processYamlDirectory(paths.src.items, [...paths.build.statics, "items"], { recursive: true })
log.ok(`Built ${items.length} items`)

// Generate items pagination
log.info("Generating items pagination...")
const pagination = system.pagination
const totalItemPages = Math.ceil(items.length / pagination)
await write([...paths.build.statics, "items", "meta.json"], {
    children: items.length,
    pages: totalItemPages
})
for (let page = 1; page <= totalItemPages; page++) {
    const start = (page - 1) * pagination
    const pageItems = items.slice(start, start + pagination)
    await write([...paths.build.statics, "items", `${page}.json`], pageItems)
}
log.ok(`Generated ${totalItemPages} item pages`)

// Generate tags pagination
log.info("Generating tags pagination...")
const tagsList = Array.from(allTags).sort()
const totalTagPages = Math.ceil(tagsList.length / pagination)
await write([...paths.build.statics, "tags", "meta.json"], {
    children: tagsList.length,
    pages: totalTagPages
})

// Generate paginated tag list files
for (let page = 1; page <= totalTagPages; page++) {
    const start = (page - 1) * pagination
    const pageTags = tagsList.slice(start, start + pagination)
    await write([...paths.build.statics, "tags", `${page}.json`], pageTags)
}

// Generate per-tag item lists with pagination
for (const tag of tagsList) {
    const tagItems = []
    for (const itemName of items) {
        const meta = await load([...paths.build.statics, "items", itemName, "meta.json"])
        if (meta?.tags?.includes(tag)) {
            tagItems.push(itemName)
        }
    }

    const tagPages = Math.ceil(tagItems.length / pagination)
    await write([...paths.build.statics, "tags", tag, "meta.json"], {
        children: tagItems.length,
        pages: tagPages
    })

    for (let page = 1; page <= tagPages; page++) {
        const start = (page - 1) * pagination
        const pageItems = tagItems.slice(start, start + pagination)
        await write([...paths.build.statics, "tags", tag, `${page}.json`], pageItems)
    }
}
log.ok(`Generated ${totalTagPages} tag pages and ${tagsList.length} tag-specific pagination structures`)

// Build sites
log.info("Building sites (YAML → JSON)...")
const siteDirs = await dir(paths.src.sites)
await processYamlDirectory(paths.src.sites, [...paths.build.statics, "sites"], { recursive: true })
log.ok(`Built ${siteDirs.length} sites`)

// Copy assets
await copyAssets([
    { src: paths.src.core, dest: paths.build.core, label: "core folder" },
    { src: paths.src.UI, dest: paths.build.UI, label: "UI folder" },
    { src: paths.src.importmap, dest: [...paths.build.root, "importmap.json"], label: "importmap.json" },
    { src: ["node_modules", "bootstrap-icons", "icons"], dest: [...paths.build.root, "images", "icons"], label: "bootstrap icons" }
])

// Copy Gun.js library files to GDB folder
log.info("Copying Gun.js library to GDB...")
const gunFiles = ["gun.js", "sea.js", ["lib", "radix.js"], ["lib", "radisk.js"], ["lib", "rindexed.js"]]
for (const filePath of gunFiles) {
    const src = Array.isArray(filePath) ? ["node_modules", "@akaoio", "gun", ...filePath] : ["node_modules", "@akaoio", "gun", filePath]
    const dest = [...paths.build.core, "GDB", Array.isArray(filePath) ? filePath[filePath.length - 1] : filePath]
    await copy(src, dest)
}
log.ok(`Copied 5 Gun.js files to GDB`)

// Build routes list using regex pattern and post-process
log.info("Building routes list...")
const found = await dir(paths.src.routes, /index\.js$/)
// Keep only directories that have index.js by stripping the suffix
const routeDirs = Array.from(new Set(found
    .filter(p => p.endsWith('index.js'))
    .map(p => p.replace(/\/index\.js$/, ''))))
await write([...paths.build.statics, "routes.json"], routeDirs)
log.ok(`Built routes list with ${routeDirs.length} routes`)

// Process i18n
log.info("Processing i18n files...")
const localeCount = await processI18n(locales)
log.ok(`Created ${localeCount} locale files`)

// Generate routes
log.info("Generating routes...")
const indexContent = await load(paths.src.index)
const routeCount = await generateRoutes(locales, items, allTags, indexContent)
log.ok(`Created ${routeCount} route files`)

// Copy geo data if it doesn't exist
const geoDestPath = [...paths.build.statics, "geo"]
if (!(await exist(geoDestPath))) {
    log.info("Copying geo data to build...")
    const startTime = Date.now()
    const fileCount = copyGeoWithProgress("./geo/data", path.join(...geoDestPath))
    const duration = ((Date.now() - startTime) / 1000).toFixed(1)
    log.ok(`Copied ${fileCount.toLocaleString()} geo files (took ${duration}s)`)
} else {
    log.ok("Using existing geo data")
}

// Generate hash files for all JSON files in build directory (excluding geo)
log.info("Generating hash files...")
const hashResult = await generateHashFiles(paths.build.root, ["geo"])
log.ok(`Created ${hashResult.hashFiles} hash files and ${hashResult.hashDatabase} hash database entries`)

// Summary
log.section("========================================")
console.log(`${icons.done} ${color.ok("Locales")}: ${locales.length}`)
console.log(`${icons.done} ${color.ok("Items")}: ${items.length}`)
console.log(`${icons.done} ${color.ok("Unique Tags")}: ${allTags.size}`)
console.log(`${icons.done} ${color.ok("Routes Created")}: ${routeCount}`)
console.log(`${icons.done} ${color.ok("Gun.js Files")}: 5`)
console.log(`${icons.done} ${color.ok("Hash Files")}: ${hashResult.hashFiles}`)
console.log(`${icons.done} ${color.ok("Hash Database")}: ${hashResult.hashDatabase}`)
log.section("========================================")
log.start("Build completed successfully!")
