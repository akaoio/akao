import { sha256 } from "../../src/core/Utils/crypto.js"

// Pen key format: <timestamp>:<pub>:<nonce>
// timestamp is Date.now() in milliseconds; candle is derived from it in the soul.
// pub is the full writer public key; nonce is iterated until PoW passes.
// nonce is iterated until SHA-256(full key) starts with difficulty zeros
export async function key() {
    const stamp = Date.now()
    const base = `${stamp}:${this.pair.pub}`
    const nonce = await pownonce(base, 3)
    return `${base}:${nonce}`
}

// Returns first nonce where sha256(full key) starts with `difficulty` zero chars.
// Yields event loop every 1000 iterations to avoid blocking the main thread.
async function pownonce(base, difficulty) {
    const prefix = "0".repeat(difficulty)
    let nonce = 0
    while (true) {
        const n = nonce.toString(36)
        if (sha256(`${base}:${n}`).startsWith(prefix)) return n
        nonce++
        if (nonce % 1000 === 0) await new Promise(r => setTimeout(r, 0))
    }
}
