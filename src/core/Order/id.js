// Construct and return order id
export function id({ maker, side, base } = {}) {
    // The expected id format:
    // <timestamp>:<makerpub>:<side>:<basetype>/<basecategory>/<baseid>/<baseqty>
    let $maker = maker || this?.maker
    $maker = $maker?.pub ? $maker.pub : null
    const $side = side || this?.side
    let $base = base || this?.base
    if ($base.type && $base?.id && $base?.quantity) {
        const category = $base.type == "item" ? $base.game : $base.type == "currency" ? $base.chain : null
        if (category) $base = `${$base.type}/${category}/${$base.id}:${$base.quantity}`
    }
    if (!["buy", "sell"].includes($side) || !$maker || !$base) throw new Error("invalidInput")
    const timestamp = Date.now()
    return `${timestamp}:${$maker}:${$side}:${$base}`
}