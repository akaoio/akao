import template from "./template.js"
import { render } from "/core/UI.js"

const TEST_MODULES = [
    "/core/tests/Events.test.js",
    "/core/tests/States.test.js",
    "/core/tests/Utils.test.js",
    "/core/tests/Router.test.js",
    "/core/tests/Forex.test.js",
    "/core/tests/IDB.test.js",
    "/core/tests/DB.test.js",
    "/core/tests/Cart.test.js",
    "/core/tests/UI.test.js",
    "/core/tests/Context.test.js",
    "/core/tests/Access.test.js",
    "/core/tests/WebAuthn.test.js",
]

export class TEST extends HTMLElement {
    constructor() {
        super()
        this._results = []   // { suiteName, tests: [...], status }
        this._totals = { passed: 0, failed: 0, skipped: 0, total: 0 }
        this._running = false
        this._openSuites = new Set()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }

    connectedCallback() {
        const root = this.shadowRoot
        root.getElementById("run-all").addEventListener("click", () => this._runAll())
        root.getElementById("run-failed").addEventListener("click", () => this._runFailed())
        this._runAll()
    }

    async _runAll(filter) {
        if (this._running) return
        this._running = true
        this._results = []
        this._totals = { passed: 0, failed: 0, skipped: 0, total: 0 }
        this._openSuites.clear()
        this.shadowRoot.getElementById("suites").innerHTML = ""
        this._updateSummary()

        // Load test modules fresh (import cache — no re-run needed, Test.reset() handles state)
        const { default: Test } = await import("/core/Test.js")
        Test.reset()
        for (const mod of TEST_MODULES) 
            try { await import(mod) } catch (e) { console.error("Failed to load", mod, e) }
        

        await Test.run(filter || null, (suiteResult) => {
            this._onSuiteComplete(suiteResult)
        })

        this._running = false
    }

    async _runFailed() {
        const failedSuites = this._results
            .filter(s => s.tests.some(t => t.status === "fail"))
            .map(s => s.name)
        if (!failedSuites.length) return
        // Re-run by filter (suite name match)
        for (const name of failedSuites) await this._runAll(name)
    }

    _onSuiteComplete(suiteResult) {
        const existing = this._results.findIndex(s => s.name === suiteResult.name)
        if (existing >= 0) 
            this._results[existing] = suiteResult
         else 
            this._results.push(suiteResult)
        

        for (const t of suiteResult.tests) {
            if (t.status === "pass") this._totals.passed++
            else if (t.status === "fail") this._totals.failed++
            else if (t.status === "skip") this._totals.skipped++
            this._totals.total++
        }

        this._updateSummary()
        this._renderSuite(suiteResult)
    }

    _updateSummary() {

        const root = this.shadowRoot
        const { passed, failed, skipped, total } = this._totals

        // Counts
        const summary = root.getElementById("summary")

        // Remove old count badges
        for (const el of summary.querySelectorAll(".count")) el.remove()

        const h1 = summary.querySelector("h1")
        if (passed > 0) {
            const el = document.createElement("span")
            el.className = "count passed"
            el.textContent = `${passed} passed`
            h1.after(el)
        }
        if (failed > 0) {
            const el = document.createElement("span")
            el.className = "count failed"
            el.textContent = `${failed} failed`
            h1.after(el)
        }
        if (skipped > 0) {
            const el = document.createElement("span")
            el.className = "count skipped"
            el.textContent = `${skipped} skipped`
            h1.after(el)
        }

        // Progress bar
        const pct = total > 0 ? Math.round((passed + failed + skipped) / total * 100) : 0
        const fill = summary.querySelector(".fill")
        fill.style.width = `${pct}%`
        fill.classList.toggle("has-failures", failed > 0)
    }

    _renderSuite(suite) {
        const root = this.shadowRoot
        const suitesEl = root.getElementById("suites")

        // Remove existing element for this suite (re-render)
        const existing = suitesEl.querySelector(`[data-suite="${CSS.escape(suite.name)}"]`)
        if (existing) existing.remove()

        const hasFail = suite.tests.some(t => t.status === "fail")
        const isInteractive = suite.tests.every(t => t.interactive)
        const isOpen = this._openSuites.has(suite.name) || hasFail

        const passCount = suite.tests.filter(t => t.status === "pass").length
        const total = suite.tests.length
        const badgeText = isInteractive ? "interactive" : `${passCount}/${total}`
        const badgeClass = isInteractive ? "interactive" : hasFail ? "has-fail" : "all-pass"

        const suiteEl = document.createElement("div")
        suiteEl.className = "suite"
        suiteEl.dataset.suite = suite.name

        const headerEl = document.createElement("div")
        headerEl.className = `suite-header${isOpen ? " open" : ""}`
        headerEl.innerHTML = `
            <span class="arrow">▶</span>
            <span class="suite-name">${this._esc(suite.name)}</span>
            <span class="badge ${badgeClass}">${this._esc(badgeText)}</span>
        `

        const testsEl = document.createElement("div")
        testsEl.className = `suite-tests${isOpen ? " visible" : ""}`

        if (isOpen) this._openSuites.add(suite.name)

        headerEl.addEventListener("click", () => {
            const open = testsEl.classList.toggle("visible")
            headerEl.classList.toggle("open", open)
            if (open) this._openSuites.add(suite.name)
            else this._openSuites.delete(suite.name)
        })

        for (const test of suite.tests) {
            const row = document.createElement("div")
            const icon = { pass: "✓", fail: "✗", skip: "○", pending: "⏸" }[test.status] || "·"
            row.className = `test-row ${test.status}`

            let inner = `
                <span class="icon">${icon}</span>
                <span class="name">${this._esc(test.name)}</span>
            `

            if (test.interactive) 
                inner += `<button class="run-btn" data-test="${this._esc(test.name)}">Run</button>`
            

            if (test.status === "fail" && test.error) {
                const msg = this._esc(test.error.message || String(test.error))
                inner += `<span class="error-msg">${msg}</span>`
            }

            row.innerHTML = inner

            if (test.interactive) {
                const btn = row.querySelector(".run-btn")
                btn.addEventListener("click", () => this._runInteractive(suite.name, test.name, row))
            }

            testsEl.appendChild(row)
        }

        suiteEl.appendChild(headerEl)
        suiteEl.appendChild(testsEl)
        suitesEl.appendChild(suiteEl)
    }

    async _runInteractive(suiteName, testName, rowEl) {
        const { default: Test } = await import("/core/Test.js")
        rowEl.className = "test-row running"
        rowEl.querySelector(".icon").textContent = "…"
        try {
            const result = await Test.runOne(suiteName, testName)
            rowEl.className = `test-row ${result.status}`
            const icon = { pass: "✓", fail: "✗" }[result.status] || "·"
            rowEl.querySelector(".icon").textContent = icon
            if (result.status === "fail" && result.error) {
                let errEl = rowEl.querySelector(".error-msg")
                if (!errEl) {
                    errEl = document.createElement("span")
                    errEl.className = "error-msg"
                    rowEl.appendChild(errEl)
                }
                errEl.textContent = result.error.message || String(result.error)
            }
        } catch (e) {
            rowEl.className = "test-row fail"
            rowEl.querySelector(".icon").textContent = "✗"
        }
    }

    _esc(str) {
        return String(str)
            .replace(/&/g, "&amp;")
            .replace(/</g, "&lt;")
            .replace(/>/g, "&gt;")
            .replace(/"/g, "&quot;")
    }
}

customElements.define("route-test", TEST)

export default TEST
