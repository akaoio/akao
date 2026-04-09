// Either party files a dispute — writes to own Gun namespace
export async function dispute({ tradeId, reason, evidence = null }) {
    await this.gun.user().get("trades").get(tradeId).put({
        disputed: true,
        disputeReason: reason,
        disputeEvidence: evidence,
        disputedAt: Date.now()
    })
}
