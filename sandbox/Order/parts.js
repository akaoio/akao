import { candle } from "./candle.js"

export function parts(key) {
    const segs = String(key || "").split(":")
    if (segs.length !== 3) return null
    const [stamp, pub, nonce] = segs
    if (!stamp || !pub || !nonce) return null
    const time = Number(stamp)
    if (!Number.isInteger(time) || time <= 0) return null
    return { stamp: time, pub, nonce, candle: candle(time) }
}
