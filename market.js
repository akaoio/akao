import Gun from "@akaoio/zen"
import "@akaoio/zen/sea.js"
import { FS } from "./src/core/FS.js"
import http from "http"

const env = process.env.NODE_ENV || "development"
const pairs = await FS.load("pairs.json").catch(() => null)

const domains = await FS.load(["src", "statics", "domains.yaml"])
const preferredDomain = process.env.SITE || (env === "production" ? "mimiza.com" : "localhost")
const siteName = domains?.[preferredDomain] || "localhost"
const site = await FS.load(["src", "statics", "sites", siteName, "configs.yaml"])

const peers = Array.isArray(site?.peers) ? site.peers : []
const marketPair = pairs?.market
const marketPub = marketPair?.pub || site?.market?.pub

if (!marketPub) {
    throw new Error("Missing market pub key. Provide pairs.market.pub or site.market.pub.")
}

const PORT = process.env.GUN_PORT || 8765
const RELAY_URL = `http://127.0.0.1:${PORT}/gun`

async function alive(url) {
    try {
        const response = await fetch(url)
        return response.ok
    } catch {
        return false
    }
}

if (await alive(RELAY_URL)) {
    console.log(`Gun relay already running at ${RELAY_URL}`)
    process.exit(0)
}

const server = http.createServer(Gun.serve("./build"))
server.listen(PORT, () => console.log(`Gun relay: http://localhost:${PORT}/gun`))

export const db = Gun({ web: server, peers })

const fiats = await FS.load(["src", "statics", "fiats.yaml"])

export const rates = {}

for (const fiat of fiats) {
    rates[fiat.code] = rates[fiat.code] || Object.fromEntries(fiats.map((e) => [e.code, e.code == fiat.code ? 1 : undefined]))
}

export const filter = (data) => {
    if (typeof data === "object") {
        for (const [code, value] of Object.entries(data)) {
            const fiat = fiats.find((e) => e.code == code)
            if (!fiat) delete data[code]
            if (!value || !fiat) continue
            if (typeof value === "object") data[code] = filter(value)
            if (typeof value === "string") data[code] = Number(value)
        }
    }
    return data
}

export const coingate = async () => {
    const data = await fetch("https://api.coingate.com/v2/rates")
        .then((res) => res.json())
        .catch((error) => console.error(error))
    if (typeof data === "object" && data?.merchant) {
        const rates = filter(data.merchant)
        return rates
    }
}

export const erapi = async () => {
    const data = await fetch("https://open.er-api.com/v6/latest")
        .then((res) => res.json())
        .catch((error) => console.error(error))
    if (typeof data === "object" && data?.rates) {
        const rates = filter(data.rates)
        return { USD: { ...rates } }
    }
}

export const update = (data = {}) => {
    for (const base in rates) {
        if (!rates[base]) rates[base] = {}
        rates[base][base] = 1
        for (const quote in rates[base]) {
            if (!rates[quote]) rates[quote] = {}
            rates[quote][quote] = 1
            if (data?.[base]?.[quote]) {
                rates[base][quote] = data[base][quote]
                rates[quote][base] = 1 / data[base][quote]
            }
            const bridge = Object.values(rates).find((bridge) => bridge[base] && bridge[quote])
            if (bridge) rates[base][quote] = bridge[quote] / bridge[base]
        }
    }
}

const loop = async () => {
    let data = null
    try {
        data = await coingate()
        if (data) update(data)
        data = await erapi()
        if (data) update(data)
        const node = db.get(`~${marketPub}`).get("forex")
        if (marketPair) node.put(rates, null, { opt: { authenticator: marketPair } })
        else node.put(rates)
    } catch (error) {
        console.error(error)
    }
    setTimeout(loop, 60000 * 60)
}

loop()
