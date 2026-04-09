// Query order book — returns observable Gun set
// candle defaults to current 5-min window
export function fetch({ gun, item, type, candle = Math.floor(Date.now() / 300000) } = {}) {
    const prefix = `${candle}:${item}:${type}:`
    // TODO: use Pen soul + LEX range query
    return gun.get(item).map({ ".": { "*": prefix } })
}
