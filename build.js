#!/usr/bin/env node

const args = process.argv.slice(2)
const target = args[0]

if (!target || !["core", "crypto", "geo"].includes(target)) {
    console.error("Usage: node build.js <core|crypto|geo> [options]")
    console.error("  core - Build core application (items, routes, i18n, etc.)")
    console.error("  crypto - Build crypto statics (ABIs, chains, contracts)")
    console.error("  geo  - Build geo data from GeoNames")
    console.error("")
    console.error("Examples:")
    console.error("  node build.js core")
    console.error("  node build.js crypto")
    console.error("  node build.js geo --country=US")
    process.exit(1)
}

const modulePath = `./builder/${target}.js`

if (target === "geo" && !args.includes("--build") && !args.includes("--fix")) {
    process.argv.push("--build")
}

// Re-execute with the appropriate module
import(modulePath)
    .then((module) => {
        // Module will handle its own execution
    })
    .catch((error) => {
        console.error(`Failed to load ${target} builder:`, error)
        process.exit(1)
    })
