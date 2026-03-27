import Thread from "/core/Thread.js"
import { randomInt } from "/core/Utils.js"
import { Construct } from "/core/Construct.js"
import { Indexes } from "/core/Stores.js"

const thread = new Thread()

const ALPHABET = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
const SHARD_CHUNK = /^[A-Za-z0-9]{1,2}$/
const NEW_PUB = /^[A-Za-z0-9]{88}$/
const LOOP_DELAY = [3000, 25000]
const SCAN_BUDGET_PER_CYCLE = 24
const RESCAN_SOUL_AFTER = 5 * 60 * 1000
const RESCAN_ROOT_PAIR_AFTER = 20 * 60 * 1000

const state = {
    queue: [[]],
    scannedSouls: new Map(),
    scannedRootPairs: new Map(),
    deterministicCursor: 0,
    rootSeeded: false
}

function isPub(pub) {
    return typeof pub === "string" && NEW_PUB.test(pub)
}

function normalizePub(pub) {
    if (typeof pub !== "string") return
    pub = pub.trim()
    if (!isPub(pub)) return
    return pub
}

function pairFromCursor(cursor = 0) {
    const base = ALPHABET.length
    const max = base * base
    const id = ((cursor % max) + max) % max
    const a = Math.floor(id / base)
    const b = id % base
    return `${ALPHABET[a]}${ALPHABET[b]}`
}

function randomPair() {
    return `${ALPHABET[randomInt(0, ALPHABET.length)]}${ALPHABET[randomInt(0, ALPHABET.length)]}`
}

function soulFromPath(path = []) {
    if (!Array.isArray(path) || !path.length) return "~"
    return `~/${path.join("/")}`
}

function pubFromSoul(soul) {
    if (typeof soul !== "string") return
    if (!soul.startsWith("~")) return
    const pub = soul.slice(1)
    return normalizePub(pub)
}

function parseMaybeJSON(data) {
    if (typeof data !== "string") return data
    const text = data.trim()
    if (!text || (text[0] !== "{" && text[0] !== "[")) return data
    try {
        return JSON.parse(text)
    } catch {
        return data
    }
}

function extractLink(data) {
    data = parseMaybeJSON(data)
    if (!data || typeof data !== "object") return

    if (typeof data["#"] === "string") return data["#"]

    const relation = parseMaybeJSON(data[":"])
    if (!relation) return
    if (typeof relation === "string") return relation
    if (relation && typeof relation === "object" && typeof relation["#"] === "string") return relation["#"]
}

function extractPub(data, path = [], key) {
    const candidate = parseMaybeJSON(data)

    if (typeof candidate === "string") 
        return normalizePub(candidate) || pubFromSoul(candidate)
    

    if (!candidate || typeof candidate !== "object") {
        const rebuilt = `${path.join("")}${key || ""}`
        return normalizePub(rebuilt)
    }

    if (typeof candidate["*"] === "string") {
        const starPub = normalizePub(candidate["*"])
        if (starPub) return starPub
    }

    const directLink = extractLink(candidate)
    const linkPub = pubFromSoul(directLink)
    if (linkPub) return linkPub

    const rebuilt = `${path.join("")}${key || ""}`
    return normalizePub(rebuilt)
}

function shouldRescan(map, id, ttl) {
    const previous = map.get(id) || 0
    if (Date.now() - previous < ttl) return false
    map.set(id, Date.now())
    return true
}

function enqueue(path = []) {
    if (!Array.isArray(path)) return
    if (path.length && !path.every((part) => SHARD_CHUNK.test(part))) return
    const soul = soulFromPath(path)
    if (!shouldRescan(state.scannedSouls, soul, RESCAN_SOUL_AFTER)) return
    state.queue.push(path)
}

async function readNode(path = []) {
    const soul = soulFromPath(path)
    return await new Promise((resolve) => {
        try {
            globalThis.gun.get(soul).once((data) => resolve(data))
        } catch {
            resolve(undefined)
        }
    })
}

async function storeDiscoveredPub(pub, meta = {}) {
    pub = normalizePub(pub)
    if (!pub) return

    const data = {
        pub,
        soul: meta.soul,
        key: meta.key,
        at: Date.now()
    }

    await Indexes.Lives.get("discovery").get("pubs").get(pub).put(data)
    thread.send({ Lives: { discovery: { pubs: { [pub]: data } } } })
}

async function scanPath(path = []) {
    const soul = soulFromPath(path)
    const data = await readNode(path)
    if (!data || typeof data !== "object") return

    for (const [key, value] of Object.entries(data)) {
        if (key === "_" || !key) continue

        const pub = extractPub(value, path, key)
        if (pub) await storeDiscoveredPub(pub, { soul, key })

        const link = extractLink(value)
        if (link && link.startsWith("~/")) {
            const next = link
                .slice(2)
                .split("/")
                .filter(Boolean)
            enqueue(next)
        }

        const chunkPath = [...path, key]
        if (!SHARD_CHUNK.test(key)) continue
        if (chunkPath.join("").length >= 88) continue
        enqueue(chunkPath)
    }
}

async function seedRootByMap() {
    if (state.rootSeeded) return
    state.rootSeeded = true

    let ref
    await new Promise((resolve) => {
        const timeout = setTimeout(() => {
            if (ref && typeof ref.off === "function") ref.off()
            resolve(true)
        }, 1500)

        try {
            globalThis.gun.get("~").map().once((value, key, message, event) => {
                ref = event
                if (!key || key === "_") return
                if (!SHARD_CHUNK.test(key)) return
                enqueue([key])
                const pub = extractPub(value, [], key)
                if (pub) storeDiscoveredPub(pub, { soul: "~", key })
            })
        } catch {
            clearTimeout(timeout)
            resolve(true)
        }
    })
}

function scheduleRootPair() {
    const useDeterministic = Math.random() < 0.7
    const pair = useDeterministic ? pairFromCursor(state.deterministicCursor++) : randomPair()
    if (!pair || !SHARD_CHUNK.test(pair)) return
    if (!shouldRescan(state.scannedRootPairs, pair, RESCAN_ROOT_PAIR_AFTER)) return
    enqueue([pair])
}

async function scanCycle() {
    await seedRootByMap()
    scheduleRootPair()

    let count = 0
    while (count < SCAN_BUDGET_PER_CYCLE && state.queue.length) {
        const path = state.queue.shift()
        await scanPath(path)
        count++
    }

    await Indexes.Lives.get("discovery").get("meta").get("cursor").put(state.deterministicCursor)
    return { discovery: { queue: state.queue.length, cursor: state.deterministicCursor, scanned: count } }
}

thread.init = async () => {
    await Construct.GDB()
    await Promise.all(Object.values(Indexes).map((index) => index.ready))

    const savedCursor = Number(await Indexes.Lives.get("discovery").get("meta").get("cursor").once())
    if (Number.isFinite(savedCursor) && savedCursor >= 0) state.deterministicCursor = savedCursor

    setTimeout(() => {
        import("/core/Utils.js").then(({ loop }) => {
            loop({
                process: scanCycle,
                delay: LOOP_DELAY
            })
        })
    }, randomInt(500, 5000))
}
