#!/usr/bin/env node

const args = process.argv.slice(2)
const target = args[0]

if (!target || !["core", "geo"].includes(target)) {
    console.error("Usage: node test.js <core|geo> [options]")
    console.error("  core - Test core build (items, routes, i18n) - outputs to temp/")
    console.error("  geo  - Test geo data build and validate - outputs to temp/geo/")
    console.error("")
    console.error("Examples:")
    console.error("  node test.js core")
    console.error("  node test.js geo --country=US")
    console.error("  node test.js geo  (validates all countries in temp/geo)")
    process.exit(1)
}

async function testCore() {
    console.log(`\n${"=".repeat(70)}`)
    console.log("CORE TEST: Building to temp/")
    console.log("=".repeat(70) + "\n")

    // Import core build dependencies
    const { write, load, copy, dir, remove, isDirectory, exist } = await import("./src/core/FS.js")
    const { log } = await import("./builder/core/logger.js")
    const { generateRoutes } = await import("./builder/core/routes.js")
    const { processI18n } = await import("./builder/core/i18n.js")
    const { generateHashFiles } = await import("./builder/core/hash.js")
    const { Forex } = await import("./src/core/Forex.js")
    
    // Use temp/ as output base
    const outputBase = "temp"
    const paths = {
        src: {
            root: ["src"],
            statics: ["src", "statics"],
            items: ["src", "statics", "items"],
            sites: ["src", "statics", "sites"],
            games: ["src", "statics", "games"],
            routes: ["src", "UI", "routes"],
            core: ["src", "core"],
            UI: ["src", "UI"],
            importmap: ["importmap.json"],
            index: ["src", "index.html"]
        },
        build: {
            root: [outputBase],
            statics: [outputBase, "statics"],
            core: [outputBase, "core"],
            UI: [outputBase, "UI"]
        }
    }

    try {
        // Clean temp folder
        log.info("Cleaning temp folder...")
        await remove([outputBase])
        log.ok("Cleaned temp folder")

        // Load configuration
        log.info("Loading configuration...")
        const localesConfig = await load([...paths.src.statics, "locales.yaml"])
        const locales = localesConfig.map(locale => locale.code)
        const system = await load([...paths.src.statics, "system.yaml"]) || { pagination: 10 }

        // Load items — 2-level aware (core-managed flat + game-derived nested)
        const itemDirs = await dir(paths.src.items)
        const coreItems = []
        const gameItemsMap = {}
        const allTags = new Set()

        for (const name of itemDirs) {
            const meta = await load([...paths.src.items, name, 'meta.yaml'])
            if (meta) {
                coreItems.push(name)
                meta.tags?.forEach(tag => allTags.add(tag))
            } else {
                const subDirs = await dir([...paths.src.items, name])
                const nested = []
                for (const sub of subDirs) {
                    if (await isDirectory([...paths.src.items, name, sub])) {
                        const subMeta = await load([...paths.src.items, name, sub, 'meta.yaml'])
                        if (subMeta) { nested.push(sub); subMeta.tags?.forEach(t => allTags.add(t)) }
                    }
                }
                if (nested.length > 0) gameItemsMap[name] = nested
            }
        }
        const items = coreItems // alias for legacy references below

        log.ok(`Loaded: ${locales.length} locales, ${coreItems.length} items, ${allTags.size} tags`)

        // Load games
        const gameDirs = await dir(paths.src.games)
        const games = []

        for (const name of gameDirs) {
            const meta = await load([...paths.src.games, name, "meta.yaml"])
            if (meta) games.push(name)
        }

        // Build static files
        log.info("Building static files...")
        const staticFiles = await dir(paths.src.statics)
        const yamlFiles = staticFiles.filter(f => f.endsWith('.yaml') || f.endsWith('.yml'))
        for (const file of yamlFiles) {
            const data = await load([...paths.src.statics, file])
            if (data) {
                const jsonName = file.replace(/\.(yaml|yml)$/, '.json')
                await write([...paths.build.statics, jsonName], data)
            }
        }
        log.ok(`Built ${yamlFiles.length} static files`)

        // Build items (core-managed flat + game-derived nested)
        log.info("Building items...")
        for (const item of coreItems) {
            const itemFiles = await dir([...paths.src.items, item])
            for (const file of itemFiles) {
                const sourcePath = [...paths.src.items, item, file]
                if (await isDirectory(sourcePath)) {
                    await copy(sourcePath, [...paths.build.statics, "items", item, file])
                    continue
                }

                const data = await load(sourcePath)
                if (!data) continue

                const jsonName = file.replace(/\.(yaml|yml)$/, '.json')
                await write([...paths.build.statics, "items", item, jsonName], data)
            }
        }
        for (const [gameId, itemIds] of Object.entries(gameItemsMap)) {
            for (const itemId of itemIds) {
                const itemFiles = await dir([...paths.src.items, gameId, itemId])
                for (const file of itemFiles) {
                    const sourcePath = [...paths.src.items, gameId, itemId, file]
                    if (await isDirectory(sourcePath)) {
                        await copy(sourcePath, [...paths.build.statics, "items", gameId, itemId, file])
                        continue
                    }

                    const data = await load(sourcePath)
                    if (!data) continue

                    const jsonName = file.replace(/\.(yaml|yml)$/, '.json')
                    await write([...paths.build.statics, "items", gameId, itemId, jsonName], data)
                }
            }
        }
        log.ok(`Built ${items.length} items`)

        // Copy core and UI
        await copy(paths.src.core, paths.build.core)
        await copy(paths.src.UI, paths.build.UI)
        log.ok("Copied core and UI folders")

        // Process i18n
        log.info("Processing i18n...")
        const localeCount = await processI18n(locales, outputBase)
        log.ok(`Created ${localeCount} locale files`)

        // Generate routes
        log.info("Generating routes...")
        const routeFiles = await dir(paths.src.routes, /index\.js$/)
        const routeDirs = Array.from(new Set(routeFiles
            .filter(p => p.endsWith("index.js"))
            .map(p => p.replace(/\/index\.js$/, "")))).sort((a, b) => {
                const aDyn = a.split("/")[0]?.startsWith("[")
                const bDyn = b.split("/")[0]?.startsWith("[")
                if (aDyn && !bDyn) return 1
                if (!aDyn && bDyn) return -1
                return a.localeCompare(b)
            })
        await write([...paths.build.statics, "routes.json"], routeDirs)
        const indexContent = await load(paths.src.index)
        const routeCount = await generateRoutes(locales, items, allTags, games, indexContent, outputBase, routeDirs, gameItemsMap)
        log.ok(`Created ${routeCount} route files`)

        // Generate hash files
        log.info("Generating hash files...")
        const hashResult = await generateHashFiles(paths.build.root, ["geo"])
        log.ok(`Created ${hashResult.hashFiles} hash files`)

        console.log("\n" + "=".repeat(70))
        console.log("✅ CORE TEST PASSED: Build completed successfully!")
        console.log("=".repeat(70) + "\n")
        process.exit(0)
    } catch (error) {
        console.error("\n" + "=".repeat(70))
        console.error("❌ CORE TEST FAILED:", error.message)
        console.error("=".repeat(70) + "\n")
        console.error(error)
        process.exit(1)
    }
}

async function testGeo() {
    const { downloadGeoData, buildGeo } = await import("./builder/geo/index.js")
    const { readFileSync, readdirSync, statSync, existsSync } = await import("fs")
    const { join } = await import("path")

    function walkDir(dir, callback) {
        if (!existsSync(dir)) return
        const entries = readdirSync(dir)
        for (const entry of entries) {
            const fullPath = join(dir, entry)
            const stat = statSync(fullPath)
            if (stat.isDirectory()) {
                walkDir(fullPath, callback)
            } else if (entry.endsWith(".json") && entry !== "countries.json" && entry !== "features.json") {
                callback(fullPath)
            }
        }
    }

    function loadAllRecords(baseDir = "temp/geo") {
        const records = new Map()
        walkDir(baseDir, (filePath) => {
            try {
                const content = readFileSync(filePath, "utf-8")
                const data = JSON.parse(content)
                records.set(data.id, data)
            } catch (error) {
                // Skip invalid files
            }
        })
        return records
    }

    const countryArg = args.find((arg) => arg.startsWith("--country="))
    const targetCountry = countryArg ? countryArg.split("=")[1].toUpperCase() : null

    try {
        if (targetCountry) {
            // Test specific country
            console.log(`\n${"=".repeat(70)}`)
            console.log(`GEO TEST: Building ${targetCountry}`)
            console.log("=".repeat(70) + "\n")

            // Download data if needed
            await downloadGeoData()

            // Build test data
            await buildGeo({
                isTestMode: true,
                testCountry: targetCountry,
                outputBase: "temp"
            })

            // Validate
            console.log(`\n${"=".repeat(70)}`)
            console.log(`GEO TEST: Validating ${targetCountry}`)
            console.log("=".repeat(70) + "\n")

            const records = loadAllRecords("temp/geo")
            console.log(`✓ Loaded ${records.size.toLocaleString()} records\n`)

            const { validateCountry } = await import("./builder/geo/validate.js")
            const result = await validateCountry(targetCountry, records)

            if (result.issues === 0) {
                console.log("✅ GEO TEST PASSED: No validation issues!\n")
                process.exit(0)
            } else {
                console.log(`❌ GEO TEST FAILED: ${result.issues} validation issues\n`)
                process.exit(1)
            }
        } else {
            // Validate all countries
            console.log(`\n${"=".repeat(70)}`)
            console.log("GEO TEST: Validating all countries in temp/geo/")
            console.log("=".repeat(70) + "\n")

            const records = loadAllRecords("temp/geo")
            console.log(`✓ Loaded ${records.size.toLocaleString()} records\n`)

            const countries = new Set()
            for (const record of records.values()) {
                if (record.countryCode) countries.add(record.countryCode)
            }

            const sortedCountries = Array.from(countries).sort()
            console.log(`Found ${sortedCountries.length} countries: ${sortedCountries.join(", ")}\n`)

            let totalIssues = 0
            const { validateCountry } = await import("./builder/geo/validate.js")
            
            for (const countryCode of sortedCountries) {
                const result = await validateCountry(countryCode, records)
                totalIssues += result.issues
            }

            console.log("\n" + "=".repeat(70))
            console.log(`SUMMARY: ${sortedCountries.length} countries validated, ${totalIssues} total issues`)
            console.log("=".repeat(70) + "\n")

            if (totalIssues === 0) {
                console.log("✅ GEO TEST PASSED: All validated!\n")
                process.exit(0)
            } else {
                console.log(`❌ GEO TEST FAILED: ${totalIssues} issues\n`)
                process.exit(1)
            }
        }
    } catch (error) {
        console.error("❌ GEO TEST FAILED:", error.message)
        console.error(error)
        process.exit(1)
    }
}

if (target === "core") {
    testCore().catch((error) => {
        console.error("Test failed:", error)
        process.exit(1)
    })
} else if (target === "geo") {
    testGeo().catch((error) => {
        console.error("Test failed:", error)
        process.exit(1)
    })
}
