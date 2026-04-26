import chokidar from "chokidar"
import { spawn } from "child_process"
import { promises as fs, readFileSync, existsSync, mkdirSync, writeFileSync } from "fs"
import path from "path"
import http from "http"
import https from "https"
import os from "os"
import { URL } from "url"
import YAML from "yaml"
import selfsigned from "selfsigned"

const SRC_ROOT = "src"
const BUILD_ROOT = "build"

const HOST = process.env.HOST || "::"
const PORT = 8080
const CLI_ARGS = process.argv.slice(2)
const HAS_HTTPS_FLAG = CLI_ARGS.includes("--https")
const HTTPS_ENABLED = HAS_HTTPS_FLAG || ["1", "true", "yes", "on"].includes((process.env.HTTPS || "").toLowerCase())
const SSL_KEY = process.env.SSL_KEY || process.env.HTTPS_KEY || (CLI_ARGS.find(arg => arg.startsWith("--ssl-key=")) || "").split("=").slice(1).join("=")
const SSL_CERT = process.env.SSL_CERT || process.env.HTTPS_CERT || (CLI_ARGS.find(arg => arg.startsWith("--ssl-cert=")) || "").split("=").slice(1).join("=")
const DEV_CERT_DIR = ".certs"
const DEV_CERT_KEY_PATH = path.join(DEV_CERT_DIR, "dev-key.pem")
const DEV_CERT_CERT_PATH = path.join(DEV_CERT_DIR, "dev-cert.pem")
const DEV_EVENTS_PATH = "/__dev_events"
const DEV_CLIENT_MARKER = "__shop_dev_sse_client__"

const FULL_CORE_REBUILD_PATHS = [
    /^src[\\/]index\.html$/,
    /^src[\\/]statics[\\/]locales\.ya?ml$/,
    /^src[\\/]statics[\\/]system\.ya?ml$/,
    /^src[\\/]statics[\\/]domains\.ya?ml$/
]


const cryptoPaths = /^src[\\/]statics[\\/](chains|ABIs)[\\/]/

let buildInProgress = false
const pendingChanges = new Map()
const sseClients = new Set()

function normalizePath(filePath) {
    return filePath.replace(/\\/g, "/").replace(/^\.\//, "")
}

function toPlatformPath(filePath) {
    return filePath.replace(/\//g, path.sep)
}

function shouldFullRebuild(normalizedPath) {
    return FULL_CORE_REBUILD_PATHS.some(pattern => pattern.test(normalizedPath))
}



function isYamlFile(filePath) {
    return /\.(yaml|yml)$/i.test(filePath)
}

function toJsonTargetPath(targetPath) {
    return targetPath.replace(/\.(yaml|yml)$/i, ".json")
}

function toBuildPath(normalizedPath) {
    if (normalizedPath === "importmap.json") return path.join(BUILD_ROOT, "importmap.json")
    if (!normalizedPath.startsWith("src/")) return null
    return path.join(BUILD_ROOT, toPlatformPath(normalizedPath.slice("src/".length)))
}

async function exists(filePath) {
    try {
        await fs.access(filePath)
        return true
    } catch {
        return false
    }
}

async function ensureDir(filePath) {
    await fs.mkdir(path.dirname(filePath), { recursive: true })
}

async function writeJsonFromYaml(srcPath, destPath) {
    const yamlContent = await fs.readFile(srcPath, "utf8")
    const parsed = YAML.parse(yamlContent)
    await ensureDir(destPath)
    await fs.writeFile(destPath, `${JSON.stringify(parsed, null, 4)}\n`, "utf8")
}

async function copyOrConvert({ event, normalizedPath }) {
    const srcPath = toPlatformPath(normalizedPath)
    const rawDestPath = toBuildPath(normalizedPath)
    if (!rawDestPath) return null

    const outputPath = isYamlFile(normalizedPath) ? toJsonTargetPath(rawDestPath) : rawDestPath

    if (event === "unlink") {
        if (await exists(outputPath)) await fs.unlink(outputPath)
        return outputPath
    }

    if (isYamlFile(normalizedPath)) await writeJsonFromYaml(srcPath, outputPath)
    else {
        await ensureDir(outputPath)
        await fs.copyFile(srcPath, outputPath)
    }

    return outputPath
}

async function walkFiles(dirPath, basePath = dirPath, files = []) {
    const entries = await fs.readdir(dirPath, { withFileTypes: true })
    for (const entry of entries) {
        const fullPath = path.join(dirPath, entry.name)
        if (entry.isDirectory()) await walkFiles(fullPath, basePath, files)
        else files.push(normalizePath(path.relative(process.cwd(), fullPath)))
    }
    return files
}

async function syncInitialItems() {
    const itemsRoot = path.join(SRC_ROOT, "statics", "items")
    if (!await exists(itemsRoot)) return

    const files = await walkFiles(itemsRoot)
    for (const normalizedPath of files) await copyOrConvert({ event: "add", normalizedPath })

    console.log(`📦 Synced ${files.length} item source files into build/statics/items`)
}

function runBuild(script) {
    return new Promise((resolve, reject) => {
        const build = spawn("npm", ["run", script], { stdio: "inherit", shell: true })
        build.on("close", (code) => {
            if (code !== 0) {
                reject(new Error(`${script} failed with code ${code}`))
                return
            }
            resolve()
        })
    })
}

async function injectDevClient(htmlContent) {
    if (htmlContent.includes(DEV_CLIENT_MARKER)) return htmlContent

    // Inline HMR bootstrap (runs BEFORE any ES modules load)
    const bootstrap = `<script data-dev-client="${DEV_CLIENT_MARKER}">
// HMR Bootstrap - Runs synchronously before ANY ES modules load
(function() {
    globalThis._dev = (globalThis._dev && typeof globalThis._dev === "object") ? globalThis._dev : {};
    globalThis._dev.enabled = true;
    if (!window.customElements) return;
    
    // Initialize minimal HMR state (no __ prefix)
    window.hmr = {
        elements: new Map(),
        origdefine: customElements.define.bind(customElements),
        
        // Register component class with module URL
        reg(url, cls) {
            if (cls && typeof cls === 'function') cls._module = url;
        }
    };
    
    // Install customElements.define interceptor
    customElements.define = function(tag, cls, opts) {
        // Auto-detect module URL from call stack
        let moduleUrl = cls._module || cls.module || null;
        
        // If not explicitly set, try to extract from call stack
        if (!moduleUrl) {
            const stack = new Error().stack;
            if (stack) {
                const lines = stack.split('\\n');
                for (let i = 1; i < Math.min(lines.length, 5); i++) {
                    const line = lines[i];
                    const match = line.match(/(https?:\\/\\/[^:\\s)]+\\.js)/);
                    if (match) {
                        moduleUrl = match[1].split('?')[0];
                        break;
                    }
                }
            }
        }
        
        window.hmr.elements.set(tag, {
            class: cls,
            module: moduleUrl
        });
        return window.hmr.origdefine(tag, cls, opts);
    };
    
    console.log('🔌 HMR: Interceptor installed');
})();
</script>`

    // Read HMR client code from file (SSE + full runtime loader)
    let hmrClientCode = ""
    const hmrClientPath = path.join("build", "core", "HMR", "client.js")
    if (await exists(hmrClientPath)) hmrClientCode = await fs.readFile(hmrClientPath, "utf8")

    const clientLoader = `<script type="module">
${hmrClientCode}
</script>`

    // Inject bootstrap FIRST (in head), then client loader
    const injected = bootstrap + clientLoader

    if (/<script type="module" src="\/core\/Launcher\.js"/i.test(htmlContent))
        // Inject right before Launcher.js
        return htmlContent.replace(/<script type="module" src="\/core\/Launcher\.js"/i, `${injected}\n    <script type="module" src="/core/Launcher.js"`)

    if (/<\/head>/i.test(htmlContent)) return htmlContent.replace(/<\/head>/i, `${injected}</head>`)
    if (/<\/body>/i.test(htmlContent)) return htmlContent.replace(/<\/body>/i, `${injected}</body>`)
    return `${htmlContent}\n${injected}`
}

function broadcastReload() {
    for (const client of sseClients)
        try {
            client.write("data: reload\n\n")
        } catch {
            sseClients.delete(client)
        }
}

function broadcastHMR(update) {
    const message = JSON.stringify(update)
    for (const client of sseClients)
        try {
            client.write(`data: ${message}\n\n`)
        } catch {
            sseClients.delete(client)
        }
}

function determineUpdateType(normalizedPath) {
    if (normalizedPath.includes('/template.js')) return 'template'
    if (normalizedPath.endsWith('.css.js')) return 'css'
    if (normalizedPath.endsWith('.js')) return 'js'
    if (normalizedPath.endsWith('.json')) return 'json'
    if (normalizedPath.endsWith('.yaml') || normalizedPath.endsWith('.yml')) return 'yaml'
    return 'other'
}

function getMimeType(filePath) {
    const ext = path.extname(filePath).toLowerCase()
    const map = {
        ".html": "text/html; charset=utf-8",
        ".js": "text/javascript; charset=utf-8",
        ".css": "text/css; charset=utf-8",
        ".json": "application/json; charset=utf-8",
        ".svg": "image/svg+xml",
        ".png": "image/png",
        ".jpg": "image/jpeg",
        ".jpeg": "image/jpeg",
        ".webp": "image/webp",
        ".gif": "image/gif",
        ".ico": "image/x-icon",
        ".txt": "text/plain; charset=utf-8",
        ".hash": "text/plain; charset=utf-8",
        ".wasm": "application/wasm"
    }
    return map[ext] || "application/octet-stream"
}

function getAllLanIPs() {
    const interfaces = os.networkInterfaces()
    const ips = new Set(["127.0.0.1"])

    for (const list of Object.values(interfaces)) {
        if (!Array.isArray(list)) continue
        for (const net of list) {
            if (!net || net.family !== "IPv4") continue
            ips.add(net.address)
        }
    }

    return Array.from(ips)
}

function formatHost(host) {
    return host.includes(":") ? `[${host}]` : host
}

async function loadHttpsCredentials() {
    if (SSL_KEY && SSL_CERT)
        return {
            key: readFileSync(path.resolve(SSL_KEY)),
            cert: readFileSync(path.resolve(SSL_CERT)),
            source: "env"
        }

    const resolvedKeyPath = path.resolve(DEV_CERT_KEY_PATH)
    const resolvedCertPath = path.resolve(DEV_CERT_CERT_PATH)

    if (existsSync(resolvedKeyPath) && existsSync(resolvedCertPath))
        return {
            key: readFileSync(resolvedKeyPath),
            cert: readFileSync(resolvedCertPath),
            source: "cache"
        }

    if (!existsSync(path.resolve(DEV_CERT_DIR))) mkdirSync(path.resolve(DEV_CERT_DIR), { recursive: true })

    const altNames = [{ type: 2, value: "localhost" }, ...getAllLanIPs().map((ip) => ({ type: 7, ip }))]

    const generated = await selfsigned.generate([{ name: "commonName", value: "localhost" }], {
        keySize: 2048,
        algorithm: "sha256",
        days: 365,
        extensions: [{ name: "subjectAltName", altNames }]
    })

    writeFileSync(resolvedKeyPath, generated.private, "utf8")
    writeFileSync(resolvedCertPath, generated.cert, "utf8")

    return {
        key: Buffer.from(generated.private, "utf8"),
        cert: Buffer.from(generated.cert, "utf8"),
        source: "generated"
    }
}

async function resolveBuildFile(urlPathname) {
    const pathname = decodeURIComponent(urlPathname)
    const extensionlessRequest = !path.extname(pathname)
    let requestPath = pathname === "/" ? "/index.html" : pathname
    if (requestPath.endsWith("/")) requestPath += "index.html"

    const safeRelativePath = requestPath.replace(/^\/+/, "")
    let absolutePath = path.resolve(BUILD_ROOT, safeRelativePath)
    const buildRootResolved = path.resolve(BUILD_ROOT)

    if (!absolutePath.startsWith(buildRootResolved)) return null

    if (await exists(absolutePath)) {
        const stats = await fs.stat(absolutePath)
        if (stats.isDirectory()) {
            const indexPath = path.join(absolutePath, "index.html")
            if (await exists(indexPath)) absolutePath = indexPath
        }
        return absolutePath
    }

    if (extensionlessRequest) {
        const htmlPath = `${absolutePath}.html`
        if (await exists(htmlPath)) return htmlPath
        const indexPath = path.join(absolutePath, "index.html")
        if (await exists(indexPath)) return indexPath

        // SPA fallback: unmatched page routes serve root index.html
        const fallback = path.join(buildRootResolved, "index.html")
        if (await exists(fallback)) return fallback
    }

    return null
}

async function startStaticServer() {
    const server = http.createServer(async (req, res) => {
        try {
            const base = `http://${formatHost(HOST)}:${PORT}`
            const requestUrl = new URL(req.url || "/", base)

            if (requestUrl.pathname === "/__dev_reload") {
                broadcastReload()
                res.writeHead(204)
                res.end()
                return
            }

            if (requestUrl.pathname === DEV_EVENTS_PATH) {
                res.writeHead(200, {
                    "Content-Type": "text/event-stream",
                    "Cache-Control": "no-cache, no-transform",
                    Connection: "keep-alive"
                })
                res.write(": connected\n\n")
                sseClients.add(res)

                req.on("close", () => {
                    sseClients.delete(res)
                })
                return
            }

            const filePath = await resolveBuildFile(requestUrl.pathname)

            if (!filePath) {
                res.writeHead(404, { "Content-Type": "text/plain; charset=utf-8" })
                res.end("Not Found")
                return
            }

            const mimeType = getMimeType(filePath)
            const coiHeaders = {
                "Cross-Origin-Opener-Policy": "same-origin",
                "Cross-Origin-Embedder-Policy": "credentialless",
                "Cache-Control": "no-store, no-cache, must-revalidate",
                Pragma: "no-cache",
                Expires: "0"
            }
            if (mimeType.startsWith("text/html")) {
                const content = await fs.readFile(filePath, "utf8")
                const withClient = await injectDevClient(content)
                res.writeHead(200, { "Content-Type": mimeType, ...coiHeaders })
                res.end(withClient)
                return
            }

            const content = await fs.readFile(filePath)
            res.writeHead(200, { "Content-Type": mimeType, ...coiHeaders })
            res.end(content)
        } catch {
            res.writeHead(500, { "Content-Type": "text/plain; charset=utf-8" })
            res.end("Internal Server Error")
        }
    })

    const protocol = HTTPS_ENABLED ? "https" : "http"

    if (HTTPS_ENABLED) {
        const { key, cert, source } = await loadHttpsCredentials()
        const secureServer = https.createServer({ key, cert }, server.listeners("request")[0])
        secureServer.listen(PORT, HOST)
        return { server: secureServer, protocol, httpsSource: source }
    }

    server.listen(PORT, HOST)
    return { server, protocol, httpsSource: null }
}

function getDevUrls(protocol = "http") {
    const interfaces = os.networkInterfaces()
    const urls = new Set([`${protocol}://localhost:${PORT}`])

    for (const list of Object.values(interfaces)) {
        if (!Array.isArray(list)) continue
        for (const net of list) {
            if (!net || net.family !== "IPv4" || net.internal) continue
            urls.add(`${protocol}://${net.address}:${PORT}`)
        }
    }

    return Array.from(urls)
}

async function handleChange({ event, normalizedPath }) {
    if (cryptoPaths.test(normalizedPath)) {
        console.log(`⛓️ Crypto source changed (${normalizedPath}) -> running build:crypto...`)
        await runBuild("build:crypto")
        return
    }

    if (shouldFullRebuild(normalizedPath)) {
        console.log(`🏗️ Global source changed (${normalizedPath}) -> running build:core...`)
        await runBuild("build:core")
        return
    }

    if (normalizedPath === "importmap.json" || normalizedPath.startsWith("src/")) {
        const output = await copyOrConvert({ event, normalizedPath })
        if (output) console.log(`⚡ Incremental ${event}: ${normalizedPath} -> ${normalizePath(output)}`)
        
    }
}

function enqueueChange(event, filePath) {
    const normalizedPath = normalizePath(filePath)

    if (!normalizedPath.startsWith("src/") && normalizedPath !== "importmap.json") return

    pendingChanges.set(normalizedPath, { event, normalizedPath })
    void processQueue()
}

async function processQueue() {
    if (buildInProgress || pendingChanges.size === 0) return
    buildInProgress = true

    try {
        while (pendingChanges.size > 0) {
            const batch = Array.from(pendingChanges.values())
            pendingChanges.clear()
            let batchChanged = false
            const hmrUpdates = []

            const hasGlobalChange = batch.some(({ normalizedPath }) => shouldFullRebuild(normalizedPath))
            if (hasGlobalChange) {
                console.log("🏗️ Detected global changes in batch -> running build:core once...")
                await runBuild("build:core")
                batchChanged = true
                if (batchChanged) broadcastReload()
                continue
            }

            const hasCryptoChange = batch.some(({ normalizedPath }) => cryptoPaths.test(normalizedPath))
            if (hasCryptoChange) {
                console.log("⛓️ Detected crypto changes in batch -> running build:crypto once...")
                await runBuild("build:crypto")
                batchChanged = true
            }

            for (const change of batch) {
                if (cryptoPaths.test(change.normalizedPath)) continue
                await handleChange(change)
                batchChanged = true
                
                // Determine HMR update type
                const updateType = determineUpdateType(change.normalizedPath)
                const buildPath = toBuildPath(change.normalizedPath)
                
                if (buildPath && (updateType === 'js' || updateType === 'css' || updateType === 'template')) {
                    // Convert to browser-accessible path
                    const browserPath = normalizePath(buildPath).replace(/^build\//, '/')
                    
                    hmrUpdates.push({
                        type: 'hmr',
                        path: browserPath,
                        updateType,
                        timestamp: Date.now()
                    })
                }
            }

            // Send HMR updates or fallback to full reload
            if (hmrUpdates.length > 0) {
                console.log(`🔥 HMR: Broadcasting ${hmrUpdates.length} update(s)`)
                for (const update of hmrUpdates) broadcastHMR(update)
            } else if (batchChanged) broadcastReload()
            
        }
        console.log("✅ Incremental rebuild completed")
    } catch (error) {
        console.error("❌ Rebuild failed:", error.message)
    } finally {
        buildInProgress = false
        if (pendingChanges.size > 0) void processQueue()
    }
}

const watcher = chokidar.watch(["src/**/*", "importmap.json"], {
    ignored: /(^|[\/\\])\../,
    persistent: true,
    ignoreInitial: true
})

watcher.on("add", (filePath) => enqueueChange("add", filePath))
watcher.on("change", (filePath) => enqueueChange("change", filePath))
watcher.on("unlink", (filePath) => enqueueChange("unlink", filePath))

// Start live-server with file watcher
console.log("🚀 Starting development server with incremental auto-rebuild...")
await syncInitialItems()

const { protocol, httpsSource } = await startStaticServer()

console.log(`✅ Server listening on ${HOST}:${PORT} (${protocol.toUpperCase()})`)
if (HTTPS_ENABLED) {
    console.log("🔐 HTTPS enabled (secure context)")
    if (httpsSource === "generated") console.log(`🧪 Generated local dev certificate at ${DEV_CERT_CERT_PATH}`)

    if (httpsSource === "cache") console.log(`📄 Using cached local dev certificate at ${DEV_CERT_CERT_PATH}`)
    
}
for (const url of getDevUrls(protocol)) console.log(`🌐 ${url}`)
console.log("👀 Watching src/ for file-based rebuilds...")
