import Wallet from "./Wallet.js"
import Ethers from "./Ethers.js"
import { sha256 } from "./Utils/crypto.js"

export class Lock {
    constructor({ payer, escrow, recipient, entropy = null } = {}) {
        if (!payer || !escrow || !recipient) return { error: "invalidInput" }
        this.payer = payer
        this.escrow = escrow
        this.recipient = recipient
        this.entropy = entropy
        this.secret = this.secret.bind(this)
        this.address = this.address.bind(this)
    }
    
    async secret() {
        const { sea } = globalThis
        if (!sea) throw new Error("Lock: SEA not initialized")
        const secret = await sea.secret(this.escrow.epub, this.payer.epriv)
        return secret
    }
    
    async address() {
        const { HDNodeWallet } = await import("ethers")
        if (!HDNodeWallet) throw new Error("Lock: ethers.js not available")
        
        // 1. Get payer-escrow shared secret
        const secret = await this.secret()
        
        // 2. Derive trade-specific seed: sha256(secret : tradeId)
        const entropy = sha256(secret + ":" + this.entropy)
        
        // 3. Derive child index (31-bit non-hardened)
        const index = parseInt(entropy.substring(0, 8), 16) & 0x7fffffff
        
        // 4. Derive child from recipient's xpub
        const root = HDNodeWallet.fromExtendedKey(this.recipient.xpub)
        const child = root.deriveChild(index)
        
        return child.address
    }
}