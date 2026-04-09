// Write initial trade record to both Gun user namespaces
// Each party writes to their own namespace — Gun SEA enforces authorship
export async function create({ tradeId }) {
    const record = { tradeId, status: "matched", ts: Date.now() }
    // TODO: gun.user().get("trades").get(tradeId).put(record)
    // Both maker and taker write their own side
    await this.gun.user(this.maker.pub).get("trades").get(tradeId).put(record)
    await this.gun.user(this.taker.pub).get("trades").get(tradeId).put(record)
}
