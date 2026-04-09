import { Lock } from "../Lock.js"

// Payer (maker for buy, taker for sell) deposits to TL + CL
// Sequential EVM txs — NOT atomic. Each receipt must be checked.
// If CL tx fails after TL succeeds → partial state → recovery path required
export async function deposit({ tradeId, payer, recipient, xpub, xpubAffiliate, amounts }) {
    const tl = new Lock({ payer: payer.pair, escrow: this.escrow, recipient: { xpub }, trade: tradeId, type: "TL" })
    const tlAddr = await tl.address()

    let clAddr = null
    if (xpubAffiliate) {
        const cl = new Lock({ payer: payer.pair, escrow: this.escrow, recipient: { xpub: xpubAffiliate }, trade: tradeId, type: "CL" })
        clAddr = await cl.address()
    }

    // TODO: send txs from payer wallet, verify each receipt
    // return { tl: tlAddr, cl: clAddr, txs: [...] }
    return { tl: tlAddr, cl: clAddr }
}
