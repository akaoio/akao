import { sha256 } from "../../src/core/Utils/crypto.js"

// Domain separator ":TL:" or ":CL:" isolates index space per lock type
// Prevents on-chain correlation between TL and CL of the same trade
export async function index() {
    if (this.type !== "TL" && this.type !== "CL") throw new Error("invalidLockType")
    const s = await this.secret()
    const seed = sha256(s + ":" + this.type + ":" + this.tradeId)
    return parseInt(seed.slice(0, 8), 16) & 0x7fffffff
}
