import { FS } from "../src/core/FS.js"
import { paths } from "./core/config.js"
import { log } from "./core/logger.js"

// All ngosang tracker files — fetch all, filter wss:// only (browser WebTorrent requirement)
const BASE = "https://raw.githubusercontent.com/ngosang/trackerslist/master"
const TRACKER_FILES = [
    "trackers_all_ws.txt",
    "trackers_all.txt",
    "trackers_best.txt",
]

log.start("Fetching WebSocket tracker list from ngosang/trackerslist...")

const results = await Promise.allSettled(
    TRACKER_FILES.map((file) =>
        fetch(`${BASE}/${file}`, { signal: AbortSignal.timeout(10000) }).then((res) => {
            if (!res.ok) throw new Error(`HTTP ${res.status}`)
            return res.text()
        })
    )
)

const fetched = []
for (let i = 0; i < results.length; i++)
    if (results[i].status === "fulfilled") {
        const wss = results[i].value
            .split("\n")
            .map((l) => l.trim())
            .filter((l) => l.startsWith("wss://"))
        for (const t of wss) fetched.push(t)
        log.ok(`${TRACKER_FILES[i]}: ${wss.length} wss:// trackers`)
    } else log.info(`${TRACKER_FILES[i]}: failed (${results[i].reason?.message})`)

// Hardcoded fallbacks — always included regardless of network availability
const FALLBACK = [
    "wss://tracker.btorrent.xyz",
    "wss://tracker.openwebtorrent.com",
    "wss://tracker.webtorrent.dev",
]

// Merge fetched + fallbacks, deduplicate
const trackers = [...new Set([...fetched, ...FALLBACK])]

if (trackers.length === FALLBACK.length) log.info("No new trackers fetched — using fallbacks only")
await FS.write([...paths.src.statics, "trackers.yaml"], trackers)
log.ok(`Written ${trackers.length} trackers to src/statics/trackers.yaml`)
