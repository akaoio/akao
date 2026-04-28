#!/usr/bin/env node

import http from "http"
import https from "https"
import { promises as fs, existsSync, readFileSync } from "fs"
import path from "path"
import { fileURLToPath } from "url"

const __filename = fileURLToPath(import.meta.url)
const __dirname = path.dirname(__filename)

const HOST = process.env.HOST || "::"
const HTTP_PORT = parsePort(process.env.HTTP_PORT || "80", "HTTP_PORT")
const HTTPS_PORT = parsePort(process.env.HTTPS_PORT || process.env.PORT || "443", "HTTPS_PORT")
const BUILD_ROOT = path.resolve(__dirname, "build")
const WELL_KNOWN_ROOT = path.resolve(__dirname, ".well-known")
const CERT_ROOT = path.resolve(__dirname, ".certs")
const SSL_KEY_PATH = path.resolve(process.env.SSL_KEY || process.env.HTTPS_KEY || path.join(CERT_ROOT, "prod-key.pem"))
const SSL_CERT_PATH = path.resolve(process.env.SSL_CERT || process.env.HTTPS_CERT || path.join(CERT_ROOT, "prod-cert.pem"))

if (HTTP_PORT === HTTPS_PORT) throw new Error(`HTTP_PORT (${HTTP_PORT}) and HTTPS_PORT (${HTTPS_PORT}) must be different`)
if (!existsSync(BUILD_ROOT)) throw new Error(`Missing ${BUILD_ROOT}. Run "npm run build" before starting production mode.`)
if (!existsSync(SSL_KEY_PATH) || !existsSync(SSL_CERT_PATH)) {
    throw new Error(
        [
            "Missing TLS certificate files for akao production mode.",
            `Expected key:  ${SSL_KEY_PATH}`,
            `Expected cert: ${SSL_CERT_PATH}`,
            'Set SSL_KEY and SSL_CERT (or HTTPS_KEY and HTTPS_CERT) to override these defaults.'
        ].join("\n")
    )
}

const tls = {
    key: readFileSync(SSL_KEY_PATH),
    cert: readFileSync(SSL_CERT_PATH)
}

function parsePort(value, label) {
    const port = Number.parseInt(String(value), 10)
    if (!Number.isInteger(port) || port < 1 || port > 65535) throw new Error(`${label} must be an integer between 1 and 65535`)
    return port
}

function formatBindHost(host) {
    return host.includes(":") ? `[${host}]` : host
}

function formatAuthority(hostname, port) {
    const normalized = hostname.startsWith("[") || !hostname.includes(":") ? hostname : `[${hostname}]`
    return port === 443 ? normalized : `${normalized}:${port}`
}

function getMimeType(filePath) {
    const ext = path.extname(filePath).toLowerCase()
    const map = {
        ".css": "text/css; charset=utf-8",
        ".gif": "image/gif",
        ".hash": "text/plain; charset=utf-8",
        ".html": "text/html; charset=utf-8",
        ".ico": "image/x-icon",
        ".jpeg": "image/jpeg",
        ".jpg": "image/jpeg",
        ".js": "text/javascript; charset=utf-8",
        ".json": "application/json; charset=utf-8",
        ".map": "application/json; charset=utf-8",
        ".md": "text/markdown; charset=utf-8",
        ".png": "image/png",
        ".svg": "image/svg+xml",
        ".txt": "text/plain; charset=utf-8",
        ".wasm": "application/wasm",
        ".webmanifest": "application/manifest+json; charset=utf-8",
        ".webp": "image/webp",
        ".xml": "application/xml; charset=utf-8"
    }

    return map[ext] || "application/octet-stream"
}

async function pathExists(targetPath) {
    try {
        await fs.access(targetPath)
        return true
    } catch {
        return false
    }
}

async function resolveFile(rootPath, urlPathname, { spaFallback = false } = {}) {
    const pathname = decodeURIComponent(urlPathname)
    let requestPath = pathname === "/" ? "/index.html" : pathname

    if (requestPath.endsWith("/")) requestPath += "index.html"

    const safeRelativePath = requestPath.replace(/^\/+/, "")
    const absolutePath = path.resolve(rootPath, safeRelativePath)

    if (absolutePath !== rootPath && !absolutePath.startsWith(`${rootPath}${path.sep}`)) return null

    if (await pathExists(absolutePath)) {
        const stats = await fs.stat(absolutePath)
        if (stats.isDirectory()) {
            const nestedIndex = path.join(absolutePath, "index.html")
            if (await pathExists(nestedIndex)) return nestedIndex
        } else return absolutePath
    }

    if (!spaFallback || path.extname(requestPath)) return null

    const htmlPath = `${absolutePath}.html`
    if (await pathExists(htmlPath)) return htmlPath

    const nestedIndex = path.join(absolutePath, "index.html")
    if (await pathExists(nestedIndex)) return nestedIndex

    const fallback = path.join(rootPath, "index.html")
    return (await pathExists(fallback)) ? fallback : null
}

async function serveFile(filePath, res) {
    const body = await fs.readFile(filePath)
    res.writeHead(200, {
        "Cache-Control": "no-store",
        "Content-Type": getMimeType(filePath),
        "Cross-Origin-Embedder-Policy": "credentialless",
        "Cross-Origin-Opener-Policy": "same-origin"
    })
    res.end(body)
}

async function serveAcmeChallenge(requestUrl, res) {
    const challengePath = requestUrl.pathname.replace(/^\/\.well-known/, "") || "/"
    const filePath = await resolveFile(WELL_KNOWN_ROOT, challengePath)
    if (!filePath) {
        res.writeHead(404, { "Content-Type": "text/plain; charset=utf-8" })
        res.end("Not Found")
        return true
    }

    await serveFile(filePath, res)
    return true
}

function getHostname(req) {
    const host = req.headers.host || "localhost"
    return host.replace(/:\d+$/, "")
}

function buildRedirectUrl(req, requestUrl) {
    const hostname = getHostname(req)
    return `https://${formatAuthority(hostname, HTTPS_PORT)}${requestUrl.pathname}${requestUrl.search}`
}

async function handleHttpsRequest(req, res) {
    const requestUrl = new URL(req.url || "/", `https://${formatBindHost(HOST)}:${HTTPS_PORT}`)

    if (requestUrl.pathname.startsWith("/.well-known/acme-challenge/")) {
        await serveAcmeChallenge(requestUrl, res)
        return
    }

    const filePath = await resolveFile(BUILD_ROOT, requestUrl.pathname, { spaFallback: true })
    if (!filePath) {
        res.writeHead(404, { "Content-Type": "text/plain; charset=utf-8" })
        res.end("Not Found")
        return
    }

    await serveFile(filePath, res)
}

async function handleHttpRequest(req, res) {
    const requestUrl = new URL(req.url || "/", `http://${formatBindHost(HOST)}:${HTTP_PORT}`)

    if (requestUrl.pathname.startsWith("/.well-known/acme-challenge/")) {
        await serveAcmeChallenge(requestUrl, res)
        return
    }

    res.writeHead(301, { Location: buildRedirectUrl(req, requestUrl) })
    res.end()
}

function withRequestLogging(handler) {
    return async (req, res) => {
        try {
            await handler(req, res)
        } catch (error) {
            console.error(error?.stack || error)
            if (res.headersSent) return
            res.writeHead(500, { "Content-Type": "text/plain; charset=utf-8" })
            res.end("Internal Server Error")
        }
    }
}

function listen(server, port) {
    return new Promise((resolve, reject) => {
        server.once("error", reject)
        server.listen(port, HOST, () => {
            server.off("error", reject)
            resolve()
        })
    })
}

const httpsServer = https.createServer(tls, withRequestLogging(handleHttpsRequest))
const httpServer = http.createServer(withRequestLogging(handleHttpRequest))

await Promise.all([listen(httpsServer, HTTPS_PORT), listen(httpServer, HTTP_PORT)])

console.log(`akao production server listening on https://${formatBindHost(HOST)}:${HTTPS_PORT}`)
console.log(`HTTP redirect server listening on http://${formatBindHost(HOST)}:${HTTP_PORT}`)
console.log(`Using TLS key: ${SSL_KEY_PATH}`)
console.log(`Using TLS cert: ${SSL_CERT_PATH}`)
