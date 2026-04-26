import { defineConfig, devices } from "@playwright/test"

export default defineConfig({
    testDir: "./e2e",
    fullyParallel: true,
    forbidOnly: !!process.env.CI,
    retries: process.env.CI ? 1 : 0,
    outputDir: "test-results/playwright",
    reporter: [["list"], ["json", { outputFile: "playwright-report/results.json" }]],
    use: {
        baseURL: "http://127.0.0.1:8080/en/",
        trace: "on-first-retry"
    },
    webServer: {
        command: "node preview.js",
        url: "http://127.0.0.1:8080/en/",
        reuseExistingServer: !process.env.CI,
        timeout: 30_000
    },
    projects: [
        { name: "chromium", use: { ...devices["Desktop Chrome"] } },
        { name: "firefox", use: { ...devices["Desktop Firefox"] } },
        { name: "webkit", use: { ...devices["Desktop Safari"] } }
    ]
})
