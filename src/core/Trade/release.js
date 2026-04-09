import { sha256 } from "../Utils/crypto.js"

// Platform releases funds to seller + affiliate (auto-release or dispute resolution)
// Platform recomputes indexes from payer's secret, derives recipient spending keys
export async function release({ tradeId, payer, recipient, affiliate, platpair }) {
    const { sea } = globalThis
    const secretPayer = await sea.secret(payer.epub, platpair)
    const index_TL = parseInt(sha256(secretPayer + ":TL:" + tradeId).slice(0, 8), 16) & 0x7fffffff
    const index_CL = parseInt(sha256(secretPayer + ":CL:" + tradeId).slice(0, 8), 16) & 0x7fffffff

    // TODO: write unlock indexes to Gun (as if buyer confirmed)
    // or directly execute withdrawal transactions as platform
}
