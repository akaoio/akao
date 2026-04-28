#!/usr/bin/env node

import http from "http"
import { promises as fs } from "fs"
import path from "path"

const WEBROOT = path.resolve(process.env.WEBROOT || process.cwd())
const HOST = process.env.HOST || "::"
const PORT = Number.parseInt(process.env.PORT || "80", 10)

function getRequestPath(req) {
    const rawUrl = typeof req.url === "string" && req.url ? req.url : "/"
    return rawUrl.split("?")[0] || "/"
}

function resolveChallengeFile(requestPath) {
    if (!requestPath.startsWith("/.well-known/acme-challenge/")) return null
    const absolutePath = path.resolve(WEBROOT, `.${requestPath}`)
    const allowedRoot = path.resolve(WEBROOT, ".well-known", "acme-challenge")
    if (absolutePath !== allowedRoot && !absolutePath.startsWith(`${allowedRoot}${path.sep}`)) return null
    return absolutePath
}

const server = http.createServer(async (req, res) => {
    try {
        const filePath = resolveChallengeFile(getRequestPath(req))
        if (!filePath) {
            res.writeHead(404, { "Content-Type": "text/plain; charset=utf-8" })
            res.end("Not Found")
            return
        }

        const body = await fs.readFile(filePath)
        res.writeHead(200, {
            "Cache-Control": "no-store",
            "Content-Type": "text/plain; charset=utf-8",
        })
        res.end(body)
    } catch (error) {
        if (error?.code === "ENOENT") {
            res.writeHead(404, { "Content-Type": "text/plain; charset=utf-8" })
            res.end("Not Found")
            return
        }

        console.error(error?.stack || error)
        res.writeHead(500, { "Content-Type": "text/plain; charset=utf-8" })
        res.end("Internal Server Error")
    }
})

server.listen({ host: HOST, port: PORT, ipv6Only: false }, () => {
    console.log(`ACME challenge server listening on ${HOST}:${PORT} with webroot ${WEBROOT}`)
})
