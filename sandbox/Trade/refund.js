import { Lock } from "../Lock.js"
import {
    putTradeRecord,
    resolveRoles,
    resolveTradeId,
    resolvePublishedXpub,
    rootFromSecret
} from "./helpers.js"
import zen from "../../src/core/ZEN.js"

// Platform refunds payer — requires platform authority pair
// Platform recomputes all spending keys via DH shared secrets
export async function refund({ tradeId, payer, recipient, affiliate = null, platpair = null, to = null } = {}) {
    const resolvedTradeId = await resolveTradeId(this, tradeId)
    const platformPair = platpair || this.platform?.pair
    const roles = resolveRoles(this, { payer, recipient, affiliate })
    const payerEntity = roles.payer
    const recipientEntity = roles.recipient
    const affiliateEntity = roles.affiliate
    if (!platformPair) throw new Error("platpairRequired")
    const recipientXpub = await resolvePublishedXpub({
        runtime: this.zen,
        party: recipientEntity,
        platpair: platformPair
    })

    const recipientRoot = await rootFromSecret(await zen.secret(recipientEntity.epub, platformPair))
    const tl = new Lock({
        payer: payerEntity.pair,
        platform: this.platform,
        recipient: { xpub: recipientXpub },
        tradeId: resolvedTradeId,
        type: "TL"
    })
    const unlockTL = await tl.unlock(recipientRoot.extendedKey)

    let unlockCL = null
    let unlockIndexCL = null
    if (affiliateEntity?.pub && affiliateEntity?.epub) {
        const affiliateXpub = await resolvePublishedXpub({
            runtime: this.zen,
            party: affiliateEntity,
            platpair: platformPair
        })
        const affiliateRoot = await rootFromSecret(await zen.secret(affiliateEntity.epub, platformPair))
        const cl = new Lock({
            payer: payerEntity.pair,
            platform: this.platform,
            recipient: { xpub: affiliateXpub },
            tradeId: resolvedTradeId,
            type: "CL"
        })
        unlockCL = await cl.unlock(affiliateRoot.extendedKey)
        unlockIndexCL = await cl.index()
    }

    // This resolves the deterministic refund paths and metadata,
    // but does not execute settlement on-chain yet.
    const fields = {
        refundReady: true,
        refundReadyAt: Date.now(),
        status: "refund_ready",
        refundTo: to || payerEntity.address || payerEntity.wallet?.address || null,
        unlock_index_TL: await tl.index(),
        unlock_index_CL: unlockIndexCL
    }

    if (this.platform?.pub)
        await putTradeRecord({
            runtime: this.zen,
            pub: this.platform.pub,
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
