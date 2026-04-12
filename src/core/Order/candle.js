export function candle(ts = Date.now()) {
    return Math.floor(Number(ts) / 300000)
}
