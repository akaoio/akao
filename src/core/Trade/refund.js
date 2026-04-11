import { Lock } from "../Lock.js"
import {
    putTradeRecord,
    resolveRoles,
    resolveTradeId,
    resolvePublishedXpub,
    rootFromSecret
} from "./helpers.js"

// Platform refunds payer — requires Platform pair (escrow authority)
// Platform recomputes all spending keys via DH shared secrets
export async function refund({ tradeId, payer, recipient, affiliate = null, platpair = null, to = null } = {}) {
    const resolvedTradeId = await resolveTradeId(this, tradeId)
    const platformPair = platpair || this.escrow?.pair
    const roles = resolveRoles(this, { payer, recipient, affiliate })
    const payerEntity = roles.payer
    const recipientEntity = roles.recipient
    const affiliateEntity = roles.affiliate
    const recipientXpub = await resolvePublishedXpub({
        gun: this.gun,
        party: recipientEntity,
        platpair: platformPair
    })

    if (!platformPair) throw new Error("platpairRequired")

    const recipientRoot = await rootFromSecret(await globalThis.sea.secret(recipientEntity.epub, platformPair))
    const tl = new Lock({
        payer: payerEntity.pair,
        escrow: this.escrow,
        recipient: { xpub: recipientXpub },
        trade: resolvedTradeId,
        type: "TL"
    })
    const unlockTL = await tl.unlock(recipientRoot.extendedKey)

    let unlockCL = null
    if (affiliateEntity?.pub && affiliateEntity?.epub) {
        const affiliateXpub = await resolvePublishedXpub({
            gun: this.gun,
            party: affiliateEntity,
            platpair: platformPair
        })
        const affiliateRoot = await rootFromSecret(await globalThis.sea.secret(affiliateEntity.epub, platformPair))
        const cl = new Lock({
            payer: payerEntity.pair,
            escrow: this.escrow,
            recipient: { xpub: affiliateXpub },
            trade: resolvedTradeId,
            type: "CL"
        })
        unlockCL = await cl.unlock(affiliateRoot.extendedKey)
    }

    // TODO(debt): this marks the trade as "refunded" after deriving platform-spendable keys
    // and refund metadata, but does not yet transfer funds on-chain.
    // The persisted status currently means "refund path resolved by platform", not "refund settled".
    const fields = {
        refunded: true,
        refundedAt: Date.now(),
        status: "refunded",
        refundTo: to || payerEntity.address || payerEntity.wallet?.address || null,
        unlock_index_TL: await tl.index(),
        unlock_index_CL: unlockCL ? await new Lock({
            payer: payerEntity.pair,
            escrow: this.escrow,
            recipient: { xpub: await resolvePublishedXpub({ gun: this.gun, party: affiliateEntity, platpair: platformPair }) },
            trade: resolvedTradeId,
            type: "CL"
        }).index() : null
    }

    if (this.escrow?.pub)
        await putTradeRecord({
            gun: this.gun,
            pub: this.escrow.pub,
            tradeId: resolvedTradeId,
            fields,
            pair: platformPair
        })

    return {
        tradeId: resolvedTradeId,
        ...fields,
        tl: { address: unlockTL.address },
        cl: unlockCL ? { address: unlockCL.address } : null
    }
}
