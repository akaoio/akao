/**
 * Micro test runner — works in both Node.js and browser.
 *
 * API:
 *   Test.describe("Suite name", () => { ... })
 *   Test.it("test name", async () => { ... })
 *   Test.it("interactive", async () => { ... }, { interactive: true })
 *   const results = await Test.run(filter?, onProgress?)
 *
 * Node.js: prints coloured output to console, process.exitCode = 1 on failures.
 * Browser:  returns results object; call onProgress(suiteResult) for live UI updates.
 */

import { NODE, BROWSER } from "./Utils/environment.js"

// ─── Assertion helpers ────────────────────────────────────────────────────────

export class AssertionError extends Error {
    constructor(message) {
        super(message)
        this.name = "AssertionError"
    }
}

export const assert = {
    equal(a, b) {
        if (a !== b)
            throw new AssertionError(
                `Expected ${JSON.stringify(a)} to strictly equal ${JSON.stringify(b)}`
            )
    },
    notEqual(a, b) {
        if (a === b)
            throw new AssertionError(
                `Expected values to differ but both were ${JSON.stringify(a)}`
            )
    },
    deepEqual(a, b) {
        const sa = JSON.stringify(a)
        const sb = JSON.stringify(b)
        if (sa !== sb)
            throw new AssertionError(`Expected\n  ${sa}\nto deeply equal\n  ${sb}`)
    },
    truthy(val, msg) {
        if (!val)
            throw new AssertionError(msg || `Expected ${JSON.stringify(val)} to be truthy`)
    },
    falsy(val, msg) {
        if (val)
            throw new AssertionError(msg || `Expected ${JSON.stringify(val)} to be falsy`)
    },
    /** Expect a synchronous function to throw. Returns the caught error. */
    throws(fn, msgContains) {
        let threw = false
        let caught = null
        try {
            fn()
        } catch (e) {
            threw = true
            caught = e
        }
        if (!threw) throw new AssertionError("Expected function to throw but it did not")
        if (msgContains && !caught.message?.includes(msgContains))
            throw new AssertionError(
                `Expected error message to contain "${msgContains}" but got "${caught.message}"`
            )
        return caught
    },
    /** Expect a promise to reject. Returns the rejection reason. */
    async rejects(promise, msgContains) {
        let threw = false
        let caught = null
        try {
            await promise
        } catch (e) {
            threw = true
            caught = e
        }
        if (!threw) throw new AssertionError("Expected promise to reject but it resolved")
        if (msgContains && !caught.message?.includes(msgContains))
            throw new AssertionError(
                `Expected rejection message to contain "${msgContains}" but got "${caught.message}"`
            )
        return caught
    },
    /** Assert a numeric value is within an inclusive range. */
    inRange(val, min, max) {
        if (val < min || val > max)
            throw new AssertionError(
                `Expected ${val} to be in range [${min}, ${max}]`
            )
    }
}

// ─── Registry ─────────────────────────────────────────────────────────────────

/** @type {Array<{name:string, tests:Array}>} */
let suites = []
let _currentSuite = null

function describe(name, fn, opts = {}) {
    const suite = { name, tests: [], opts }
    suites.push(suite)
    _currentSuite = suite
    fn()
    _currentSuite = null
}

function it(name, fn, opts = {}) {
    if (!_currentSuite) throw new Error(`Test.it() called outside Test.describe()`)
    // Inherit suite-level opts (browser, interactive) but let test-level opts win
    const mergedOpts = { ..._currentSuite.opts, ...opts }
    _currentSuite.tests.push({ name, fn, opts: mergedOpts })
}

/** Clear all registered suites (useful for isolated test runs). */
function reset() {
    suites = []
    _currentSuite = null
}

// ─── Runner ───────────────────────────────────────────────────────────────────

const PASS = NODE ? "\x1b[32m✓\x1b[0m" : "✓"
const FAIL = NODE ? "\x1b[31m✗\x1b[0m" : "✗"
const SKIP = NODE ? "\x1b[33m○\x1b[0m" : "○"
const CYAN  = (s) => NODE ? `\x1b[36m${s}\x1b[0m` : s
const RED   = (s) => NODE ? `\x1b[31m${s}\x1b[0m` : s
const GREEN = (s) => NODE ? `\x1b[32m${s}\x1b[0m` : s
const DIM   = (s) => NODE ? `\x1b[2m${s}\x1b[0m` : s

/**
 * Run all registered suites (or the one whose name matches `filter`).
 *
 * @param {string} [filter] - Optional substring to match against suite names
 * @param {Function} [onProgress] - Called after each suite with the suite result
 * @returns {Promise<{passed:number, failed:number, skipped:number, total:number, suites:Array}>}
 */
async function run(filter, onProgress) {
    const results = { passed: 0, failed: 0, skipped: 0, total: 0, suites: [] }

    for (const suite of suites) {
        if (filter && !suite.name.toLowerCase().includes(filter.toLowerCase())) continue

        if (NODE) console.log(`\n${CYAN("▶")} ${CYAN(suite.name)}`)

        const suiteResult = {
            name: suite.name,
            passed: 0,
            failed: 0,
            skipped: 0,
            total: suite.tests.length,
            tests: []
        }

        for (const test of suite.tests) {
            results.total++

            // Interactive tests are never auto-run
            if (test.opts.interactive) {
                suiteResult.tests.push({
                    name: test.name,
                    status: "pending",
                    interactive: true,
                    fn: test.fn
                })
                suiteResult.skipped++
                results.skipped++
                if (NODE) console.log(`  ${SKIP} ${DIM(test.name)} ${DIM("(interactive)")}`)
                continue
            }

            // Skip tests that require the browser when running in Node
            if (test.opts.browser && NODE) {
                suiteResult.tests.push({ name: test.name, status: "skip", reason: "browser only" })
                suiteResult.skipped++
                results.skipped++
                if (NODE) console.log(`  ${SKIP} ${DIM(test.name)} ${DIM("(browser only)")}`)
                continue
            }

            try {
                await test.fn()
                suiteResult.tests.push({ name: test.name, status: "pass" })
                suiteResult.passed++
                results.passed++
                if (NODE) console.log(`  ${PASS} ${test.name}`)
            } catch (e) {
                const errMsg = e?.message || String(e)
                suiteResult.tests.push({ name: test.name, status: "fail", error: errMsg })
                suiteResult.failed++
                results.failed++
                if (NODE) {
                    console.log(`  ${FAIL} ${test.name}`)
                    console.log(`      ${RED(errMsg)}`)
                }
            }
        }

        results.suites.push(suiteResult)
        if (onProgress) onProgress(suiteResult)
    }

    // ── Summary ──────────────────────────────────────────────────────────────
    if (NODE) {
        const line = "─".repeat(50)
        console.log(`\n${line}`)
        if (results.failed === 0) {
            console.log(GREEN(`  All ${results.passed} tests passed`) +
                (results.skipped ? DIM(` (${results.skipped} skipped)`) : ""))
        } else {
            console.log(
                GREEN(`  ${results.passed} passed`) + "  " +
                RED(`${results.failed} failed`) +
                (results.skipped ? DIM(`  ${results.skipped} skipped`) : "")
            )
        }
        console.log(line + "\n")

        if (results.failed > 0) process.exitCode = 1
    }

    return results
}

/**
 * Run a single interactive test by suite name + test name.
 * Returns { name, status, error? }
 */
async function runOne(suiteName, testName) {
    for (const suite of suites) {
        if (suite.name !== suiteName) continue
        for (const test of suite.tests) {
            if (test.name !== testName) continue
            try {
                await test.fn()
                return { name: test.name, status: "pass" }
            } catch (e) {
                return { name: test.name, status: "fail", error: e }
            }
        }
    }
    throw new Error(`Test "${testName}" not found in suite "${suiteName}"`)
}

const Test = { describe, it, assert, run, runOne, reset }
export default Test
export { describe, it, run, runOne, reset }
