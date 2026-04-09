// Seller marks item as delivered — writes to own Gun namespace
export async function deliver({ tradeId }) {
    await this.gun.user().get("trades").get(tradeId).put({ delivered: true, deliveredAt: Date.now() })
}
