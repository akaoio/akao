import { Lock } from "../Lock.js"
import { sha256 } from "../Utils/crypto.js"
import { HDNodeWallet, getBytes } from "../Ethers.js"

// Platform refunds payer — requires Platform pair (escrow authority)
// Platform recomputes all spending keys via DH shared secrets
export async function refund({ tradeId, payer, recipient, affiliate, platpair, to }) {
    const { sea } = globalThis
    const secretRecipient = await sea.secret(recipient.epub, platpair)
    const rootRecipient = await rootFromSecret(secretRecipient)

    const secretPayer = await sea.secret(payer.epub, platpair)
    const index_TL = parseInt(sha256(secretPayer + ":TL:" + tradeId).slice(0, 8), 16) & 0x7fffffff
    const index_CL = parseInt(sha256(secretPayer + ":CL:" + tradeId).slice(0, 8), 16) & 0x7fffffff

    // TODO: derive xprv for recipient root, unlock TL, send to payer
    // TODO: same for CL with affiliate root
}

async function rootFromSecret(s) {
    return HDNodeWallet.fromSeed(getBytes("0x" + s.padStart(64, "0")))
}
