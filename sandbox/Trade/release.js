import { Lock } from "../Lock.js"
import {
    putTradeRecord,
    resolveRoles,
    resolveTradeId,
    resolvePublishedXpub,
    rootFromSecret
} from "./helpers.js"
import zen from "../../src/core/ZEN.js"

// Platform releases funds to seller + affiliate (auto-release or dispute resolution)
// Platform recomputes indexes from payer's secret, derives recipient spending keys
export async function release({ tradeId, payer, recipient, affiliate = null, platpair = null } = {}) {
    const resolvedTradeId = await resolveTradeId(this, tradeId)
    const platformPair = platpair || this.platform?.pair
    const roles = resolveRoles(this, { payer, recipient, affiliate })
    const payerEntity = roles.payer
    const recipientEntity = roles.recipient
    const affiliateEntity = roles.affiliate
    if (!platformPair) throw new Error("platpairRequired")
    const recipientXpub = await resolvePublishedXpub({
        runtime: this.gun,
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
            runtime: this.gun,
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

    // This resolves the deterministic release paths and indexes,
    // but does not execute settlement on-chain yet.
    const fields = {
        releaseReady: true,
        releaseReadyAt: Date.now(),
        status: "release_ready",
        unlock_index_TL: await tl.index(),
        unlock_index_CL: unlockIndexCL
    }

    if (this.platform?.pub)
        await putTradeRecord({
            runtime: this.gun,
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
