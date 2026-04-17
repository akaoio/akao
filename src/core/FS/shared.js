import { NODE, BROWSER, WIN } from "../Utils/environment.js"
import { createDriver } from "./driver.js"

let fs = null
let YAML = null
if (NODE) {
    fs = await import("fs")
    YAML = await import("yaml")
}

let opfs = null
if (BROWSER) {
    const [{ OPFS }, { supportsOPFS }] = await Promise.all([
        import("../OPFS.js"),
        import("../OPFS/root.js")
    ])
    if (supportsOPFS()) opfs = new OPFS()
}

// File extensions treated as UTF-8 text — everything else is binary
export const TEXT_EXTS = ["json", "yaml", "yml", "csv", "tsv", "txt", "md", "html", "js", "css", "hash"]

export function isBinary(filePath) {
    const ext = (Array.isArray(filePath) ? filePath.at(-1) : filePath)
        .match(/\.\w+$/)?.[0]?.slice(1).toLowerCase() || ""
    return !!ext && !TEXT_EXTS.includes(ext)
}

// Builds a Node.js absolute file path from a path array.
// Cannot import join.js here (join.js imports shared.js → circular), so inline the logic.
function _nodePath(path) {
    const sep = WIN ? "\\" : "/"
    const joined = path.filter(Boolean).join(sep)
    // Guard: if already absolute (abs path or drive letter), return as-is
    if (joined.startsWith("/") || /^[A-Za-z]:/.test(joined)) return joined
    const base = globalThis._root || process.cwd()
    return base + sep + joined
}

// Unified I/O driver — env resolved ONCE at module load, never branches at call time.
// All methods accept array paths. Contract enforced by createDriver().
export const driver = createDriver(BROWSER ? {
    readBytes: async (path) => {
        const buf = await opfs?.load(path).catch(() => null)
        return buf ? new Uint8Array(buf) : null
    },
    writeBytes: async (path, bytes) => {
        if (!opfs) return
        await opfs.write(path, bytes)
        return { success: true, path: [...path].join("/") }
    },
    remove: async (path) => {
        if (!opfs) return
        const exists = await opfs.exist(path).catch(() => false)
        if (exists) await opfs.remove(path)
    },
    list: async (path) => opfs?.dir(path).catch(() => []) ?? [],
    entries: async (path) => {
        if (!opfs) return []
        try {
            const handle = await opfs.$dir(opfs._path(path))
            const result = []
            for await (const [name, entry] of handle.entries())
                result.push({ name, isDir: entry.kind === "directory" })
            return result
        } catch { return [] }
    },
    exists: async (path) => opfs?.exist(path).catch(() => false) ?? false,
    isDir: async (path) => {
        if (!opfs) return false
        try { await opfs.$dir(opfs._path(path)); return true } catch { return false }
    },
    mkdir: async (path) => { await opfs?.mkdir(path) },
    move: async (src, dst) => { await opfs?.move(src, dst) },
    copyFile: async (src, dst) => {
        const buf = await opfs.load(src)
        await opfs.write(dst, buf)
    },
} : {
    readBytes: (path) => {
        const p = _nodePath(path)
        if (!fs.existsSync(p)) return null
        return new Uint8Array(fs.readFileSync(p))
    },
    writeBytes: (path, bytes) => {
        const p = _nodePath(path)
        const sep = WIN ? "\\" : "/"
        const dir = p.slice(0, p.lastIndexOf(sep))
        if (!fs.existsSync(dir)) fs.mkdirSync(dir, { recursive: true })
        fs.writeFileSync(p, bytes)
        return { success: true, path: p }
    },
    remove: (path) => {
        const p = _nodePath(path)
        if (!fs.existsSync(p)) return
        if (fs.statSync(p).isDirectory()) fs.rmSync(p, { recursive: true, force: true })
        else fs.unlinkSync(p)
    },
    list: (path) => {
        const p = _nodePath(path)
        return fs.existsSync(p) ? fs.readdirSync(p) : []
    },
    entries: (path) => {
        const p = _nodePath(path)
        if (!fs.existsSync(p)) return []
        return fs.readdirSync(p, { withFileTypes: true })
            .map(e => ({ name: e.name, isDir: e.isDirectory() }))
    },
    exists: (path) => fs.existsSync(_nodePath(path)),
    isDir: (path) => {
        const p = _nodePath(path)
        return fs.existsSync(p) && fs.statSync(p).isDirectory()
    },
    mkdir: (path) => {
        const p = _nodePath(path)
        if (!fs.existsSync(p)) fs.mkdirSync(p, { recursive: true })
    },
    move: (src, dst) => { fs.renameSync(_nodePath(src), _nodePath(dst)) },
    copyFile: (src, dst) => {
        const srcP = _nodePath(src)
        const dstP = _nodePath(dst)
        const sep = WIN ? "\\" : "/"
        const dir = dstP.slice(0, dstP.lastIndexOf(sep))
        if (!fs.existsSync(dir)) fs.mkdirSync(dir, { recursive: true })
        fs.copyFileSync(srcP, dstP)
    },
})

export { fs, YAML, NODE, BROWSER, WIN, opfs }
