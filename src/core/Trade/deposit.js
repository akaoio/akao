import { Lock } from "../Lock.js"
import {
    putTradeRecord,
    resolveRoles,
    resolveTradeId,
    resolvePublishedXpub,
    sendValue,
    serializeReceipt
} from "./helpers.js"

// Payer (maker for buy, taker for sell) deposits to TL + CL
// Sequential EVM txs — NOT atomic. Each receipt must be checked.
// If CL tx fails after TL succeeds → partial state → recovery path required
export async function deposit({ tradeId, payer, recipient, affiliate = null, xpub = null, xpubAffiliate = null, amounts = {} } = {}) {
    const resolvedTradeId = await resolveTradeId(this, tradeId)
    const roles = resolveRoles(this, { payer, recipient, affiliate })
    const payerEntity = roles.payer
    const recipientEntity = roles.recipient
    const affiliateEntity = roles.affiliate || this.order?.affiliate || null

    if (!payerEntity?.pair) throw new Error("payerPairRequired")
    if (!recipientEntity?.pub && !xpub) throw new Error("recipientRequired")

    // TODO(debt): this only verifies "xpub exists in gun.user(pub)".
    // release()/refund() additionally require platform-derived xpub validation via platpair.
    // Until deposit() enforces the same invariant, a crafted published xpub can pass deposit
    // but fail later in platform-mediated unhappy paths.
    const recipientXpub = xpub || await resolvePublishedXpub({ gun: this.gun, party: recipientEntity })
    const affiliateXpub = xpubAffiliate || (
        affiliateEntity?.pub
            ? await resolvePublishedXpub({ gun: this.gun, party: affiliateEntity })
            : null
    )

    const tl = new Lock({
        payer: payerEntity.pair,
        platform: this.platform,
        recipient: { xpub: recipientXpub },
        tradeId: resolvedTradeId,
        type: "TL"
    })
    const tlAddr = await tl.address()

    let clAddr = null
    if (affiliateXpub) {
        const cl = new Lock({
            payer: payerEntity.pair,
            platform: this.platform,
            recipient: { xpub: affiliateXpub },
            tradeId: resolvedTradeId,
            type: "CL"
        })
        clAddr = await cl.address()
    }

    const sender = payerEntity.wallet || payerEntity.signer || payerEntity
    const tlValue = amounts?.tl ?? amounts?.TL ?? amounts?.payment ?? amounts?.amount ?? null
    const clValue = affiliateXpub ? (amounts?.cl ?? amounts?.CL ?? amounts?.commission ?? null) : null
    const txs = []

    try {
        if (tlValue != null) {
            const receipt = await sendValue(sender, { to: tlAddr, value: tlValue })
            txs.push(serializeReceipt(receipt, tlValue, tlAddr))
        }

        if (clAddr && clValue != null) {
            const receipt = await sendValue(sender, { to: clAddr, value: clValue })
            txs.push(serializeReceipt(receipt, clValue, clAddr))
        }
    } catch (error) {
        const partial = {
            status: "deposit_partial",
            deposited: false,
            depositError: error.message,
            tl: tlAddr,
            cl: clAddr,
            txs: JSON.stringify(txs)
        }
        await putTradeRecord({
            gun: this.gun,
            pub: payerEntity.pub,
            tradeId: resolvedTradeId,
            fields: partial,
            pair: payerEntity.pair
        })
        return { error: "partialDeposit", tradeId: resolvedTradeId, ...partial, txs }
    }

    if (txs.length) {
        await putTradeRecord({
            gun: this.gun,
            pub: payerEntity.pub,
            tradeId: resolvedTradeId,
            fields: {
                status: "deposited",
                deposited: true,
                depositedAt: Date.now(),
                tl: tlAddr,
                cl: clAddr,
                txs: JSON.stringify(txs)
            },
            pair: payerEntity.pair
        })
    }

    return {
        tradeId: resolvedTradeId,
        tl: tlAddr,
        cl: clAddr,
        xpub: recipientXpub,
        xpubAffiliate: affiliateXpub,
        txs
    }
}
