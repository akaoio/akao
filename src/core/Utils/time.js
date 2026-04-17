// This function returns the candle number
export function now(length = 60000) {
    const now = Date.now()
    return length <= 0 || length == 1 ? now : Math.floor(now / length)
}