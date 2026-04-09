// Merge trade state from both maker and taker Gun namespaces
// Both sides are public Gun user data — anyone with maker.pub + taker.pub can read
export async function read({ tradeId }) {
    return new Promise(resolve => {
        const state = {}
        let count = 0
        const merge = data => {
            Object.assign(state, data)
            if (++count >= 2) resolve(state)
        }
        this.gun.user(this.maker.pub).get("trades").get(tradeId).once(merge)
        this.gun.user(this.taker.pub).get("trades").get(tradeId).once(merge)
    })
}
