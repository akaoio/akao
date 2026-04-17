export function normalize({ pair, side, base, quote, affiliate = null, referrer = null, xpub = null } = {}) {
    const makerXpub = xpub || pair?.xpub || pair?.maker?.xpub || null
    const normalizedMaker = {
        pub: pair?.pub || null,
        epub: pair?.epub || null,
        xpub: makerXpub
    }
    const normalizedBase = {
        type: base?.type || "item",
        id: base?.id || null,
        quantity: base?.quantity
    }
    const normalizedQuote = {
        type: quote?.type || "crypto",
        quantity: quote?.quantity,
        contract: quote?.contract || null,
        chain: quote?.chain ?? null
    }
    const affiliatePub = affiliate?.pub || affiliate || referrer || null
    return {
        side,
        maker: normalizedMaker,
        base: normalizedBase,
        quote: normalizedQuote,
        affiliate: affiliatePub ? { pub: affiliatePub } : null
    }
}

export function validate(order = {}) {
    const { side, maker, base, quote } = order
    if (!side || !maker?.pub || !maker?.epub || !base?.id || base?.quantity === undefined || base?.quantity === null || quote?.quantity === undefined || quote?.quantity === null || !quote?.contract || quote?.chain === undefined || quote?.chain === null) throw new Error("invalidInput")
    if (!maker.xpub) throw new Error("xpubRequired")
    if (side !== "buy" && side !== "sell") throw new Error("invalidSide")
    if (base.type !== "item") throw new Error("invalidBaseType")
    if (quote.type !== "crypto") throw new Error("invalidQuoteType")
    if (typeof base.quantity !== "number" || !isFinite(base.quantity) || base.quantity <= 0) throw new Error("invalidBaseQuantity")
    if (typeof quote.quantity !== "number" || !isFinite(quote.quantity) || quote.quantity <= 0) throw new Error("invalidQuoteQuantity")
    if (base.id.includes(":")) throw new Error("invalidBaseId")
}

export function canonical(order = {}) {
    return JSON.stringify({
        side: order.side,
        maker: {
            pub: order.maker?.pub || null,
            epub: order.maker?.epub || null,
            xpub: order.maker?.xpub || null
        },
        base: {
            type: order.base?.type || null,
            id: order.base?.id || null,
            quantity: order.base?.quantity ?? null
        },
        quote: {
            type: order.quote?.type || null,
            quantity: order.quote?.quantity ?? null,
            contract: order.quote?.contract || null,
            chain: order.quote?.chain ?? null
        },
        affiliate: order.affiliate?.pub ? { pub: order.affiliate.pub } : null
    })
}

export function payload(order = {}, { orderId, status = "open" } = {}) {
    return {
        orderId,
        maker: {
            pub: order.maker.pub,
            epub: order.maker.epub,
            xpub: order.maker.xpub
        },
        side: order.side,
        base: {
            type: order.base.type,
            id: order.base.id,
            quantity: order.base.quantity
        },
        quote: {
            type: order.quote.type,
            quantity: order.quote.quantity,
            contract: order.quote.contract,
            chain: order.quote.chain
        },
        affiliate: order.affiliate?.pub ? { pub: order.affiliate.pub } : null,
        status
    }
}
