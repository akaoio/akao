// All inputs must be ASCII-safe strings (base64 SEA secrets, hex hashes)
export async function secret() {
    const { sea } = globalThis
    if (!sea) throw new Error("Lock: SEA not initialized")
    return await sea.secret(this.platform.epub, this.payer)
}
