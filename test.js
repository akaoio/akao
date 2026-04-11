#!/usr/bin/env node

import { spawn } from "child_process"
import { existsSync, readFileSync, readdirSync, statSync } from "fs"
import { join } from "path"

const args = process.argv.slice(2)
const target = args[0]

if (!target || !["core", "geo"].includes(target)) {
    console.error("Usage: node test.js <core|geo> [options]")
    console.error("  core - Build to build/ and verify generated core artifacts")
    console.error("  geo  - Build to build/geo and validate generated geo data")
    console.error("")
    console.error("Examples:")
    console.error("  node test.js core")
    console.error("  node test.js geo --country=US")
    console.error("  node test.js geo  (builds and validates build/geo)")
    process.exit(1)
}

function run(command, commandArgs) {
    return new Promise((resolve, reject) => {
        const child = spawn(command, commandArgs, { stdio: "inherit" })
        child.on("exit", (code) => (code === 0 ? resolve() : reject(new Error(`${command} ${commandArgs.join(" ")} exited with code ${code}`))))
        child.on("error", reject)
    })
}

function assert(condition, message) {
    if (!condition) throw new Error(message)
}

function readJSON(path) {
    return JSON.parse(readFileSync(path, "utf8"))
}

function walkDir(dir, callback) {
    if (!existsSync(dir)) return
    const entries = readdirSync(dir)
    for (const entry of entries) {
        const fullPath = join(dir, entry)
        const stat = statSync(fullPath)
        if (stat.isDirectory()) walkDir(fullPath, callback)
        else callback(fullPath, entry)
    }
}

function loadAllGeoRecords(baseDir = "build/geo") {
    const records = new Map()
    walkDir(baseDir, (filePath, entry) => {
        if (!entry.endsWith(".json") || entry === "countries.json" || entry === "features.json") return

        try {
            const data = readJSON(filePath)
            records.set(data.id, data)
        } catch {
            // Ignore malformed records; validation below will fail on missing data.
        }
    })
    return records
}

async function testCore() {
    console.log(`\n${"=".repeat(70)}`)
    console.log("CORE TEST: Building to build/ and verifying generated artifacts")
    console.log("=".repeat(70) + "\n")

    await run(process.execPath, ["build.js", "core"])

    const buildRoot = "build"
    const staticsRoot = join(buildRoot, "statics")
    const localesRoot = join(staticsRoot, "locales")
    const itemsRoot = join(staticsRoot, "items")
    const localeFiles = readdirSync(localesRoot).filter((file) => file.endsWith(".json"))
    const localeCodes = localeFiles.map((file) => file.replace(/\.json$/, ""))
    const routes = readJSON(join(staticsRoot, "routes.json"))
    const itemMeta = readJSON(join(itemsRoot, "meta.json"))
    const itemPages = readdirSync(itemsRoot).filter((file) => /^\d+\.json$/.test(file))
    const homeHTML = readFileSync(join(buildRoot, "en", "index.html"), "utf8")

    assert(existsSync(join(buildRoot, "index.html")), "Missing build/index.html")
    assert(existsSync(join(buildRoot, "404.html")), "Missing build/404.html")
    assert(existsSync(join(buildRoot, "importmap.json")), "Missing build/importmap.json")
    assert(existsSync(join(buildRoot, "core", "Launcher.js")), "Missing build/core/Launcher.js")
    assert(existsSync(join(buildRoot, "UI", "routes", "test", "index.js")), "Missing build/UI/routes/test/index.js")

    assert(localeFiles.length > 0, "No locale JSON files found in build/statics/locales")
    assert(localeFiles.includes("en.json"), "Missing build/statics/locales/en.json")
    for (const locale of localeCodes) assert(existsSync(join(buildRoot, locale, "index.html")), `Missing build/${locale}/index.html`)

    assert(Array.isArray(routes) && routes.length > 0, "build/statics/routes.json is empty")
    assert(routes.includes("test"), "build/statics/routes.json is missing the test route")
    assert(routes.some((route) => route.startsWith("item/")), "build/statics/routes.json is missing item routes")

    assert(Number.isInteger(itemMeta.children) && itemMeta.children > 0, "build/statics/items/meta.json has invalid children count")
    assert(Number.isInteger(itemMeta.pages) && itemMeta.pages > 0, "build/statics/items/meta.json has invalid pages count")
    assert(itemPages.length === itemMeta.pages, `build/statics/items page count mismatch: expected ${itemMeta.pages}, got ${itemPages.length}`)
    assert(homeHTML.includes('/core/Launcher.js'), "build/en/index.html does not boot through /core/Launcher.js")

    console.log(`✓ Verified ${localeFiles.length} locale payloads and locale entrypoints`)
    console.log(`✓ Verified ${routes.length} generated routes in build/statics/routes.json`)
    console.log(`✓ Verified ${itemMeta.children.toLocaleString()} item entries across ${itemMeta.pages} build/statics/items pages`)

    console.log("\n" + "=".repeat(70))
    console.log("✅ CORE TEST PASSED: build/ artifacts are structurally valid")
    console.log("=".repeat(70) + "\n")
}

async function testGeo() {
    const { validateCountry } = await import("./builder/geo/validate.js")
    const countryArg = args.find((arg) => arg.startsWith("--country="))
    const targetCountry = countryArg ? countryArg.split("=")[1].toUpperCase() : null

    console.log(`\n${"=".repeat(70)}`)
    console.log(targetCountry ? `GEO TEST: Building ${targetCountry} into build/geo` : "GEO TEST: Building and validating build/geo")
    console.log("=".repeat(70) + "\n")

    const buildArgs = ["build.js", "geo"]
    if (targetCountry) buildArgs.push(`--country=${targetCountry}`)
    await run(process.execPath, buildArgs)

    const records = loadAllGeoRecords("build/geo")
    assert(records.size > 0, "No geo records found in build/geo")
    console.log(`✓ Loaded ${records.size.toLocaleString()} records from build/geo\n`)

    if (targetCountry) {
        const result = await validateCountry(targetCountry, records)
        if (result.issues > 0) throw new Error(`${targetCountry} has ${result.issues} geo validation issues`)

        console.log("✅ GEO TEST PASSED: No validation issues!\n")
        return
    }

    const countries = new Set()
    for (const record of records.values()) if (record.countryCode) countries.add(record.countryCode)

    const sortedCountries = Array.from(countries).sort()
    console.log(`Found ${sortedCountries.length} countries: ${sortedCountries.join(", ")}\n`)

    let totalIssues = 0
    for (const countryCode of sortedCountries) {
        const result = await validateCountry(countryCode, records)
        totalIssues += result.issues
    }

    console.log("\n" + "=".repeat(70))
    console.log(`SUMMARY: ${sortedCountries.length} countries validated, ${totalIssues} total issues`)
    console.log("=".repeat(70) + "\n")

    if (totalIssues > 0) throw new Error(`Geo validation found ${totalIssues} issues`)

    console.log("✅ GEO TEST PASSED: All validated!\n")
}

const runners = { core: testCore, geo: testGeo }

runners[target]().catch((error) => {
    console.error(`❌ ${target.toUpperCase()} TEST FAILED:`, error.message)
    console.error(error)
    process.exit(1)
})
