// Pen key format: <candle>:<item_slug>:<type>:<pub8>:<nonce>
// pub8 = M.pub.slice(0,8) — Pen enforces PRE(R[5], seg3): writer's pub must start with pub8
// nonce is iterated until PoW passes (difficulty 2)
export async function key() {
    const candle = Math.floor(Date.now() / 300000)
    const pub8 = this.pair.pub.slice(0, 8)
    const base = `${candle}:${this.item}:${this.type}:${pub8}`
    const nonce = await powNonce(base, 2)
    return `${base}:${nonce}`
}

// Returns first nonce where sha256(base + nonce) starts with `difficulty` zero chars
async function powNonce(base, difficulty) {
    const { sha256 } = await import("../Utils/crypto.js")
    const prefix = "0".repeat(difficulty)
    let nonce = 0
    while (true) {
        const n = nonce.toString(36)
        if (sha256(base + n).startsWith(prefix)) return n
        nonce++
    }
}
