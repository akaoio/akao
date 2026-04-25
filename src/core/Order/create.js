import zen from "/core/ZEN.js"
import { Access } from "/core/Access.js"

export function create({ platform, maker, affiliate = null, side, base, quote } = {}) {
    const $platform = platform || this?.platform
    const $maker = maker || this?.maker
    const $side = side || this?.side
    const $base = base || this?.base
    let $quote = quote || this?.quote
    const soul = this.soul({ maker, side, base, quote })
    const id = this.id({ maker, side, base })
    if (!$quote.type || !$quote.id || !$quote.quantity) {
        const category = $quote.type == "item" ? $quote.game : $quote.type == "currency" ? $quote.chain : null
        if (category) $quote = `${$quote.type}/${category}/${$quote.id}:${$quote.quantity}`
    }
    if (!["buy", "sell"].includes($side) || !$maker || !$base || !$quote) throw new Error("invalidInput")
    const data = `${$quote}${affiliate ? `:${affiliate}` : ""}`
    return zen.get(soul).get(id).put(data, null, { authenticator: Access.get("pair")})
}