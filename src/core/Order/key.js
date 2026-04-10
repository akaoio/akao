import { sha256 } from "../Utils/crypto.js"

// Pen key format: <candle>:<item_slug>:<type>:<pub8>:<nonce>
// pub8 = pair.pub.slice(0,8) — embedded for LEX-query discovery
// nonce is iterated until SHA-256(full key) starts with difficulty zeros
export async function key() {
    const candle = Math.floor(Date.now() / 300000)
    const pub8 = this.pair.pub.slice(0, 8)
    const base = `${candle}:${this.item}:${this.type}:${pub8}`
    const nonce = await pownonce(base, 2)
    return `${base}:${nonce}`
}

// Returns first nonce where sha256(full key) starts with `difficulty` zero chars
async function pownonce(base, difficulty) {
    const prefix = "0".repeat(difficulty)
    let nonce = 0
    while (true) {
        const n = nonce.toString(36)
        if (sha256(`${base}:${n}`).startsWith(prefix)) return n
        nonce++
    }
}
