import { readFileSync, writeFileSync, existsSync, mkdirSync } from "node:fs"
import { resolve, join } from "node:path"
import { request } from "node:https"
import { parse } from "yaml"

const CONFIG_FILE = resolve("swarm.yaml")
const LOG_FILE = resolve("swarm/log.json")
const NOTES_FILE = resolve("swarm/notes.md")
const API = "https://api.telegram.org/bot"

function load() {
    if (!existsSync(CONFIG_FILE)) {
        console.error("swarm.yaml not found. Copy swarm.example.yaml and configure it.")
        process.exit(1)
    }
    return parse(readFileSync(CONFIG_FILE, "utf8"))
}

function loadlog() {
    if (!existsSync(LOG_FILE)) return { offset: 0, messages: [] }
    try { return JSON.parse(readFileSync(LOG_FILE, "utf8")) } catch { return { offset: 0, messages: [] } }
}

function savelog(data) {
    if (!existsSync("swarm")) mkdirSync("swarm", { recursive: true })
    writeFileSync(LOG_FILE, JSON.stringify(data, null, 2))
}

function get(token, method, params = {}) {
    return new Promise((ok, fail) => {
        const query = new URLSearchParams(params).toString()
        const path = `/bot${token}/${method}${query ? "?" + query : ""}`
        const req = request({ hostname: "api.telegram.org", path, method: "GET" }, res => {
            let body = ""
            res.on("data", d => body += d)
            res.on("end", () => {
                try { ok(JSON.parse(body)) } catch { fail(new Error("Invalid JSON")) }
            })
        })
        req.on("error", fail)
        req.end()
    })
}

function post(token, method, params = {}) {
    return new Promise((ok, fail) => {
        const body = JSON.stringify(params)
        const req = request({
            hostname: "api.telegram.org",
            path: `/bot${token}/${method}`,
            method: "POST",
            headers: { "Content-Type": "application/json", "Content-Length": Buffer.byteLength(body) }
        }, res => {
            let data = ""
            res.on("data", d => data += d)
            res.on("end", () => {
                try { ok(JSON.parse(data)) } catch { fail(new Error("Invalid JSON")) }
            })
        })
        req.on("error", fail)
        req.write(body)
        req.end()
    })
}

function fmt(msg, agents) {
    const who = agents?.find(a => a.username === msg.from?.username)?.name || msg.from?.username || msg.from?.first_name || "?"
    const time = new Date(msg.date * 1000).toLocaleString()
    return `[${time}] ${who}: ${msg.text || ""}`
}

async function history() {
    const config = load()
    const log = loadlog()
    const { self, groups, agents } = config

    const result = await get(self.token, "getUpdates", { offset: log.offset, limit: 100 })
    if (!result.ok) { console.error("getUpdates failed:", result.description); return }

    const groupset = new Set(groups.map(String))
    const seen = new Set(log.messages.map(m => m.message_id))
    let maxoffset = log.offset

    for (const update of result.result) {
        maxoffset = Math.max(maxoffset, update.update_id + 1)
        const msg = update.message
        if (!msg) continue
        if (!groupset.has(String(msg.chat.id))) continue
        if (seen.has(msg.message_id)) continue
        seen.add(msg.message_id)
        log.messages.push({ message_id: msg.message_id, date: msg.date, from: msg.from, text: msg.text, chat_id: msg.chat.id })
    }

    log.offset = maxoffset
    log.messages.sort((a, b) => a.date - b.date)
    savelog(log)

    const last20 = log.messages.slice(-20)
    if (!last20.length) { console.log("No messages."); return }
    console.log("\n--- Swarm History (last 20) ---")
    last20.forEach(m => console.log(fmt(m, agents)))
}

async function send(message) {
    if (!message) { console.error("Usage: node swarm.js send \"<message>\""); process.exit(1) }
    const config = load()
    const { self, groups, agents } = config

    const others = (agents || []).filter(a => a.name !== self.name)
    const mentions = others.map(a => `@${a.username}`).join(" ")
    const text = `[${self.name}] ${message}${mentions ? " " + mentions : ""}`

    const log = loadlog()
    const seen = new Set(log.messages.map(m => m.message_id))

    for (const gid of groups) {
        const res = await post(self.token, "sendMessage", { chat_id: gid, text })
        if (res.ok) {
            const msg = res.result
            if (!seen.has(msg.message_id)) {
                log.messages.push({ message_id: msg.message_id, date: msg.date, from: { username: self.name }, text: msg.text, chat_id: msg.chat.id })
                seen.add(msg.message_id)
            }
            console.log("Sent to", gid)
        } else {
            console.error("Failed to send to", gid, ":", res.description)
        }
    }

    log.messages.sort((a, b) => a.date - b.date)
    savelog(log)
}

function notes(text) {
    if (!existsSync("swarm")) mkdirSync("swarm", { recursive: true })
    if (!text) {
        if (!existsSync(NOTES_FILE)) { console.log("No notes yet."); return }
        console.log(readFileSync(NOTES_FILE, "utf8"))
        return
    }
    const entry = `\n[${new Date().toISOString()}]\n${text}\n`
    writeFileSync(NOTES_FILE, (existsSync(NOTES_FILE) ? readFileSync(NOTES_FILE, "utf8") : "") + entry)
    console.log("Note saved.")
}

async function chats() {
    const config = load()
    const result = await get(config.self.token, "getUpdates", { limit: 100 })
    if (!result.ok) { console.error("getUpdates failed:", result.description); return }
    const seen = new Map()
    for (const u of result.result) {
        const chat = u.message?.chat
        if (chat && !seen.has(chat.id)) seen.set(chat.id, chat)
    }
    if (!seen.size) { console.log("No chats found. Send a message in the group first."); return }
    console.log("\n--- Chats ---")
    seen.forEach(c => console.log(`id: ${c.id}  type: ${c.type}  title: ${c.title || c.username || c.first_name}`))
}

const [cmd, ...args] = process.argv.slice(2)
if (cmd === "history") await history()
else if (cmd === "send") await send(args.join(" "))
else if (cmd === "notes") notes(args.join(" ") || null)
else if (cmd === "chats") await chats()
else console.log("Usage: node swarm.js [history|send <message>|notes [text]|chats]")
