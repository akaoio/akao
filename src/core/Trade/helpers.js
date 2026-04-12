import { sha256 } from "../Utils/crypto.js"
import { HDNodeWallet, getBytes } from "../Ethers.js"

export function resolveRoles(trade, overrides = {}) {
    const maker = overrides.maker || trade.maker
    const taker = overrides.taker || trade.taker
    const side = trade.order?.side

    if (!maker || !taker || !side) throw new Error("invalidTradeRoles")

    const buyer = side === "buy" ? maker : taker
    const seller = side === "buy" ? taker : maker

    return {
        maker,
        taker,
        buyer,
        seller,
        payer: overrides.payer || buyer,
        recipient: overrides.recipient || seller,
        affiliate: overrides.affiliate || null
    }
}

export async function resolveTradeId(trade, tradeId = null) {
    if (tradeId) return tradeId
    return await trade.id()
}

export async function resolveOrderId(order, orderId = null) {
    if (orderId) return orderId
    if (order?.orderId) return order.orderId
    if (typeof order?.id === "function") return await order.id()
    throw new Error("orderIdRequired")
}

export async function putTradeRecord({ gun, pub, tradeId, fields, pair } = {}) {
    if (!gun || !tradeId || !fields) throw new Error("invalidTradeWrite")
    if (pub && pair) await ensureUserNamespace({ gun, pub, pair })

    return await new Promise((resolve, reject) => {
        const node = pub
            ? gun.user(pub).get("trades").get(tradeId)
            : gun.user().get("trades").get(tradeId)
        const callback = (ack) => {
            if (ack?.err) return reject(new Error(ack.err))
            resolve({ tradeId, ...fields })
        }
        if (pair) node.put(fields, callback, { opt: { authenticator: pair } })
        else node.put(fields, callback)
    })
}

export async function once(node, timeoutMs = 800) {
    return await new Promise((resolve) => {
        const timeout = setTimeout(() => resolve(undefined), timeoutMs)
        node.once((data) => {
            clearTimeout(timeout)
            resolve(data)
        })
    })
}

export async function readTradeRecord({ gun, pub, tradeId, timeoutMs = 800 } = {}) {
    if (!gun || !pub || !tradeId) throw new Error("invalidTradeRead")
    return cleanRecord(await once(gun.user(pub).get("trades").get(tradeId), timeoutMs))
}

export async function readPublishedValue({ gun, pub, field, timeoutMs = 800 } = {}) {
    if (!gun || !pub || !field) throw new Error("invalidPublishedRead")
    return await once(gun.user(pub).get(field), timeoutMs)
}

export async function rootFromSecret(secret) {
    const seed = sha256(secret)
    return HDNodeWallet.fromSeed(getBytes("0x" + seed))
}

export async function expectedXpub({ epub, platpair } = {}) {
    if (!epub || !platpair) throw new Error("invalidXpubValidation")
    const { sea } = globalThis
    if (!sea) throw new Error("Trade: SEA not initialized")
    const secret = await sea.secret(epub, platpair)
    return (await rootFromSecret(secret)).neuter().extendedKey
}

export async function resolvePublishedXpub({ gun, party, field = "xpub", platpair = null } = {}) {
    if (!party?.pub) throw new Error("partyPubRequired")
    const xpub = await readPublishedValue({ gun, pub: party.pub, field })
    if (!xpub) throw new Error("xpubNotFound")
    if (platpair && party.epub) {
        const expected = await expectedXpub({ epub: party.epub, platpair })
        if (xpub !== expected) throw new Error("invalidXpub")
    }
    return xpub
}

export function serializeReceipt(receipt, value, to) {
    return {
        hash: receipt?.hash || receipt?.transactionHash || null,
        status: receipt?.status ?? null,
        to,
        value: value == null ? null : String(value)
    }
}

export async function sendValue(sender, { to, value } = {}) {
    if (!sender || !to || value == null) throw new Error("invalidTransfer")

    if (typeof sender.sendTransaction === "function") {
        const transaction = await sender.sendTransaction({ to, value })
        return typeof transaction?.wait === "function" ? await transaction.wait() : transaction
    }

    if (typeof sender.send === "function") {
        return await sender.send({ to, amount: value })
    }

    throw new Error("unsupportedSender")
}

function cleanRecord(data) {
    if (!data || typeof data !== "object") return data
    const record = {}
    for (const [key, value] of Object.entries(data))
        if (key !== "_") record[key] = value
    return record
}

async function ensureUserNamespace({ gun, pub, pair } = {}) {
    await new Promise((resolve, reject) => {
        gun.user(pub).get("pub").put(pub, (ack) => {
            if (ack?.err) return reject(new Error(ack.err))
            resolve()
        }, { opt: { authenticator: pair } })
    })
}
