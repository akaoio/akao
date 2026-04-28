#!/usr/bin/env node
// Scan a build/ route for custom elements and shadow root depth.
// Usage: node tools/tool-kit/tools/scan-components.js <route-name>
//        node tools/tool-kit/tools/scan-components.js withdraw
//        node tools/tool-kit/tools/scan-components.js swap
// Output: shadow root map + pierce depth recommendation

import { existsSync, readFileSync, readdirSync } from "node:fs"
import { join, dirname } from "node:path"
import { fileURLToPath } from "node:url"

const __dirname = dirname(fileURLToPath(import.meta.url))
const BUILD_ROUTES = join(__dirname, "../../../build/UI/routes")
const BUILD_COMPONENTS = join(__dirname, "../../../build/UI/components")

const rawName = process.argv[2]
if (!rawName) {
    console.error("Usage: node scan-components.js <route-name>")
    console.error("       Accepts both `swap` and `route-swap`.")
    process.exit(1)
}

// Accept both `swap` and `route-swap` — strip the `route-` prefix if present.
const name = rawName.replace(/^route-/, "")

const routeDir = join(BUILD_ROUTES, name)
if (!existsSync(routeDir)) {
    console.error(`Route not found: build/UI/routes/${name}/`)
    process.exit(1)
}

function readSafe(p) {
    try { return readFileSync(p, "utf8") } catch { return "" }
}

// Find all customElements.define() calls across build/
function buildComponentRegistry() {
    const registry = new Map()
    const scan = (dir) => {
        if (!existsSync(dir)) return
        for (const entry of readdirSync(dir, { withFileTypes: true })) {
            const full = join(dir, entry.name)
            if (entry.isDirectory()) { scan(full); continue }
            if (!entry.name.endsWith(".js")) continue
            const src = readSafe(full)
            for (const m of src.matchAll(/customElements\.define\(\s*["']([^"']+)["']/g)) 
                registry.set(m[1], full.replace(join(__dirname, "../../.."), "."))
            
        }
    }
    scan(join(__dirname, "../../../build/UI"))
    return registry
}

// Extract custom element tags used in a template
function extractChildComponents(templateSrc) {
    const tags = new Set()
    for (const m of templateSrc.matchAll(/<([a-z][a-z0-9]*-[a-z0-9-]+)/g)) 
        tags.add(m[1])
    
    return [...tags]
}

// Check if a component has attachShadow
function hasShadowRoot(componentFile) {
    if (!componentFile || !existsSync(componentFile)) return false
    const src = readSafe(componentFile)
    return src.includes("attachShadow") || src.includes("shadowRoot")
}

function main() {
    const registry = buildComponentRegistry()
    const templateSrc = readSafe(join(routeDir, "template.js"))
    const indexSrc = readSafe(join(routeDir, "index.js"))

    // Find the route's own tag
    const routeTagMatch = indexSrc.match(/customElements\.define\(\s*["']([^"']+)["']/)
    const routeTag = routeTagMatch ? routeTagMatch[1] : `route-${name}`

    // Find child custom elements
    const children = extractChildComponents(templateSrc)
    const shadowChildren = children.filter(tag => {
        const file = registry.get(tag)
        return file ? hasShadowRoot(join(__dirname, "../../..", file)) : false
    })

    console.log(`\n── Shadow DOM map: ${name} ──────────────────────────────────\n`)
    console.log(`  Route component : <${routeTag}>`)
    console.log(`  Shadow root     : YES (all Route components use shadow DOM)`)
    console.log(`  Pierce depth    : ${shadowChildren.length > 0 ? "2 (DOUBLE PIERCE)" : "1 (SINGLE PIERCE)"}`)

    if (shadowChildren.length > 0) {
        console.log(`\n  Nested components with own shadow roots:`)
        for (const tag of shadowChildren) {
            const file = registry.get(tag) ?? "— not found in registry"
            console.log(`    └── <${tag}>  ${file}`)
        }
        console.log(`\n  ⚠  Double pierce required:`)
        console.log(`     document.querySelector("${routeTag}")`)
        console.log(`         ?.shadowRoot?.querySelector("${shadowChildren[0]}")`)
        console.log(`         ?.shadowRoot?.querySelector("#element")`)
    } else {
        console.log(`\n  Single pierce:`)
        console.log(`     document.querySelector("${routeTag}")`)
        console.log(`         ?.shadowRoot?.querySelector("#element")`)
    }

    // Extract IDs and named inputs from template
    const ids = [...templateSrc.matchAll(/\bid="([^"]+)"/g)].map(m => m[1])
    const namedInputs = [...templateSrc.matchAll(/name="([^"]+)"/g)].map(m => m[1])

    if (ids.length) {
        console.log(`\n  IDs found in template (use in DOM contract tests):`)
        for (const id of ids) console.log(`    #${id}`)
    }
    if (namedInputs.length) {
        console.log(`\n  Named inputs:`)
        for (const n of namedInputs) console.log(`    input[name="${n}"]`)
    }

    // Detect DeFi signals
    const defiSignals = ["estimateGas", "SendLogic", "Wallets[", "chain", "currency", "slippage", "amount"]
    const foundSignals = defiSignals.filter(s => indexSrc.includes(s))
    if (foundSignals.length) {
        console.log(`\n  DeFi signals detected → generate .web3-attack.spec.ts:`)
        for (const s of foundSignals) console.log(`    • ${s}`)
    }

    // Detect minification
    const lines = indexSrc.split("\n")
    const longLines = lines.filter(l => l.length > 500)
    if (longLines.length > 3) {
        console.log(`\n  ⚠  BUILD MAY BE MINIFIED (${longLines.length} lines > 500 chars)`)
        console.log(`     Extraction may produce unreliable selectors. Run npm run build first.`)
    }

    console.log("")
}

main()
