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

const { ready } = await import("./build/core/Launcher.js")
await ready
const { TESTS } = await import("./build/core/tests/manifest.js")
const { readdirSync, existsSync } = await import("node:fs")

// All test files — import order determines display order.
// The Node runner boots the real headless runtime through Launcher.js first.
// Source of truth for which suites belong to which environment lives in tests/manifest.js.
const testFiles = TESTS
    .filter((test) => test.node !== false)
    .map((test) => `./build/core/tests/${test.file}`)

const generatedDir = "./build/core/tests"
const generatedFiles = existsSync(generatedDir)
    ? readdirSync(generatedDir)
        .filter((file) => file.endsWith(".generated.test.js"))
        .map((file) => `./build/core/tests/${file}`)
    : []

const allTestFiles = [...new Set([...testFiles, ...generatedFiles])]

console.log("\n\x1b[1m\x1b[36m══════════════════════════════════════════════════\x1b[0m")
console.log("\x1b[1m  Browser Test Suite — Node.js runner\x1b[0m")
if (filter) console.log(`  Filter: \x1b[33m${filter}\x1b[0m`)
console.log("\x1b[1m\x1b[36m══════════════════════════════════════════════════\x1b[0m")

// Import all test files — each one registers suites via Test.describe()
for (const file of allTestFiles)
    try {
        await import(file)
    } catch (err) {
        console.error(`\x1b[31m✗ Failed to import ${file}:\x1b[0m`, err.message)
        process.exitCode = 1
    }

// Now run all registered suites
const { default: Test } = await import("./build/core/Test.js")
const results = await Test.run(filter)

// Explicit exit so Node doesn't hang on open async handles (IndexedDB init, etc.)
process.exit(process.exitCode || 0)
