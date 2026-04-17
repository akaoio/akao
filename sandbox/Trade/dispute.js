import { putTradeRecord, resolveTradeId } from "./helpers.js"

// Either party files a dispute — writes to own Zen namespace
export async function dispute({ tradeId, reason, evidence = null, party } = {}) {
    const resolvedTradeId = await resolveTradeId(this, tradeId)
    if (!party?.pub || !party?.pair) throw new Error("disputePartyRequired")

    const fields = {
        disputed: true,
        status: "disputed",
        disputeReason: reason,
        disputeEvidence: evidence,
        disputedAt: Date.now()
    }

    await putTradeRecord({
        zen: this.zen,
        pub: party.pub,
        tradeId: resolvedTradeId,
        fields,
        pair: party.pair
    })

    return { tradeId: resolvedTradeId, ...fields }
}
