#!/usr/bin/env node
/**
 * test-browser.js — Run browser-compatible test suites in Node.js.
 *
 * Only universal tests (no browser-only APIs) will run.
 * Tests marked { browser: true } or { interactive: true } are skipped automatically.
 *
 * Usage:
 *   node test-browser.js            # run all suites
 *   node test-browser.js States     # run suites matching "States"
 *   node test-browser.js Router     # run suites matching "Router"
 */

const args = process.argv.slice(2)
const filter = args[0] || null

// All test files — import order determines display order.
// Browser-only and interactive tests are gracefully skipped in Node.
// Modules that import browser-only APIs (Access, WebAuthn) are omitted here
// since they fail to load in Node even before any test runs.
const testFiles = [
    "./src/core/tests/Events.test.js",
    "./src/core/tests/States.test.js",
    "./src/core/tests/Utils.test.js",
    "./src/core/tests/Router.test.js",
    "./src/core/tests/Forex.test.js",
    "./src/core/tests/IDB.test.js",
    "./src/core/tests/DB.test.js",
    "./src/core/tests/Cart.test.js",
    "./src/core/tests/UI.test.js",
    "./src/core/tests/Context.test.js",
    "./src/core/tests/RTC.test.js",
    "./src/core/tests/Torrent.test.js",
    "./src/core/tests/Trade.test.js",
    // Access.test.js + WebAuthn.test.js — browser-only, use the /test route in browser
]

console.log("\n\x1b[1m\x1b[36m══════════════════════════════════════════════════\x1b[0m")
console.log("\x1b[1m  Browser Test Suite — Node.js runner\x1b[0m")
if (filter) console.log(`  Filter: \x1b[33m${filter}\x1b[0m`)
console.log("\x1b[1m\x1b[36m══════════════════════════════════════════════════\x1b[0m")

// Import all test files — each one registers suites via Test.describe()
for (const file of testFiles)
    try {
        await import(file)
    } catch (err) {
        console.error(`\x1b[31m✗ Failed to import ${file}:\x1b[0m`, err.message)
        process.exitCode = 1
    }

// Now run all registered suites
const { default: Test } = await import("./src/core/Test.js")
const results = await Test.run(filter)

// Explicit exit so Node doesn't hang on open async handles (IndexedDB init, etc.)
process.exit(process.exitCode || 0)
