import zen from "../../src/core/ZEN.js"

// All inputs must be ASCII-safe strings (base64 SEA secrets, hex hashes)
export async function secret() {
    return await zen.secret(this.platform.epub, this.payer)
}
