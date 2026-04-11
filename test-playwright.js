#!/usr/bin/env node

import { spawn } from "child_process"
import { setTimeout as delay } from "timers/promises"

const HOST = "127.0.0.1"
const PORT = 8080
const URL = `http://${HOST}:${PORT}/en/test`

function run(cmd, args, options = {}) {
    return new Promise((resolve, reject) => {
        const child = spawn(cmd, args, {
            stdio: "inherit",
            shell: false,
            ...options
        })

        child.on("close", (code) => {
            if (code === 0) return resolve()
            reject(new Error(`${cmd} ${args.join(" ")} exited with code ${code}`))
        })

        child.on("error", reject)
    })
}

async function wait(url, timeout = 120000) {
    const start = Date.now()
    while (Date.now() - start < timeout) {
        try {
            const response = await fetch(url)
            if (response.ok) return true
        } catch {}
        await delay(500)
    }
    throw new Error(`Timed out waiting for ${url}`)
}

async function alive(url) {
    try {
        const response = await fetch(url)
        return response.ok
    } catch {
        return false
    }
}

async function main() {
    await run("node", ["build.js", "crypto"])
    await run("node", ["build.js", "core"])

    let server = null
    if (!await alive(URL))
        server = spawn("node", ["dev.js"], {
            stdio: "inherit",
            shell: false
        })

    try {
        await wait(URL)

        const { chromium } = await import("playwright")
        const browser = await chromium.launch({ headless: true })
        const page = await browser.newPage()
        const pageErrors = []

        page.on("pageerror", (error) => {
            pageErrors.push(error?.stack || error?.message || String(error))
        })

        await page.goto(URL, { waitUntil: "domcontentloaded" })
        await page.waitForFunction(() => {
            const route = document.querySelector("route-test")
            return route?.dataset.running === "false" && Number(route?.dataset.total || 0) > 0
        }, null, { timeout: 120000 })

        const result = await page.evaluate(() => {
            const route = document.querySelector("route-test")
            return {
                mode: globalThis.launcher?.mode,
                failed: Number(route?.dataset.failed || 0),
                passed: Number(route?.dataset.passed || 0),
                skipped: Number(route?.dataset.skipped || 0),
                total: Number(route?.dataset.total || 0)
            }
        })

        await browser.close()

        if (pageErrors.length) throw new Error(`Browser runtime errors:\n${pageErrors.join("\n\n")}`)
        if (result.mode !== "browser") throw new Error(`Expected browser launcher mode, got ${result.mode}`)
        if (!result.total) throw new Error("Browser test route did not report any tests")
        if (result.failed > 0) throw new Error(`Browser test route reported ${result.failed} failed tests`)

        console.log(`Playwright browser runtime passed (${result.passed} passed, ${result.skipped} skipped)`)
    } finally {
        if (server && server.exitCode === null) server.kill("SIGTERM")
    }
}

main().catch((error) => {
    console.error(error?.stack || error)
    process.exit(1)
})
