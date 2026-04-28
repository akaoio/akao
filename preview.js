import { createServer } from "http"
import { createReadStream, existsSync, statSync } from "fs"
import { join, extname } from "path"

const BUILD = "build"
const PORT = 8080
const HOST = "127.0.0.1"

const MIME = {
    ".html": "text/html; charset=utf-8",
    ".js": "text/javascript",
    ".mjs": "text/javascript",
    ".css": "text/css",
    ".json": "application/json",
    ".wasm": "application/wasm",
    ".svg": "image/svg+xml",
    ".png": "image/png",
    ".jpg": "image/jpeg",
    ".jpeg": "image/jpeg",
    ".gif": "image/gif",
    ".webp": "image/webp",
    ".ico": "image/x-icon",
    ".woff": "font/woff",
    ".woff2": "font/woff2",
    ".txt": "text/plain",
    ".xml": "application/xml",
}

function serve(res, filePath, status = 200) {
    const mime = MIME[extname(filePath).toLowerCase()] || "application/octet-stream"
    const stream = createReadStream(filePath)

    stream.on("error", error => {
        const code = error?.code === "ENOENT" ? 404 : 500
        const body = code === 404 ? "Not Found" : "Internal Server Error"

        if (res.headersSent) {
            res.destroy()
            return
        }

        res.writeHead(code, { "Content-Type": "text/plain; charset=utf-8" })
        res.end(body)
    })

    res.writeHead(status, { "Content-Type": mime })
    stream.pipe(res)
}

const server = createServer((req, res) => {
    const pathname = new URL(req.url, "http://x").pathname

    // Exact file match
    const exact = join(BUILD, pathname)
    if (existsSync(exact) && statSync(exact).isFile()) return serve(res, exact)

    // Directory index
    const index = join(BUILD, pathname, "index.html")
    if (existsSync(index)) return serve(res, index)

    // SPA fallback to locale index
    const locale = pathname.split("/").filter(Boolean)[0]
    const fallback = join(BUILD, locale || "en", "index.html")
    if (existsSync(fallback)) return serve(res, fallback)

    // 404
    const notFound = join(BUILD, "404.html")
    if (existsSync(notFound)) return serve(res, notFound, 404)
    res.writeHead(404)
    res.end("Not Found")
})

server.listen(PORT, HOST, () => console.log(`Preview server → http://${HOST}:${PORT}`))
process.on("SIGINT", () => server.close())
process.on("SIGTERM", () => server.close())
