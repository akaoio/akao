import { sha256 } from "../../src/core/Utils/crypto.js"
import { HDNodeWallet, getBytes } from "../../src/core/Ethers.js"
import zen from "../../src/core/ZEN.js"

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

export async function putTradeRecord({ runtime = zen, gun, pub, tradeId, fields, pair } = {}) {
    runtime = runtime || gun
    if (!runtime || !tradeId || !fields) throw new Error("invalidTradeWrite")
    if (pub && pair) await ensureUserNamespace({ runtime, pub, pair })

    return await new Promise((resolve, reject) => {
        const owner = pub || pair?.pub
        if (!owner) return reject(new Error("tradeOwnerRequired"))
        const node = runtime.get("~" + owner).get("trades").get(tradeId)
        const callback = (ack) => {
            if (ack?.err) return reject(new Error(ack.err))
            resolve({ tradeId, ...fields })
        }
        if (pair) node.put(fields, callback, { authenticator: pair })
        else node.put(fields, callback)
    })
}

export async function readTradeRecord({ runtime = zen, gun, pub, tradeId, timeoutMs = 800 } = {}) {
    runtime = runtime || gun
    if (!runtime || !pub || !tradeId) throw new Error("invalidTradeRead")
    return cleanRecord(await runtime.get("~" + pub).get("trades").get(tradeId).once(timeoutMs))
}

export async function readPublishedValue({ runtime = zen, gun, pub, field, timeoutMs = 800 } = {}) {
    runtime = runtime || gun
    if (!runtime || !pub || !field) throw new Error("invalidPublishedRead")
    return await runtime.get("~" + pub).get(field).once(timeoutMs)
}

export async function rootFromSecret(secret) {
    const seed = sha256(secret)
    return HDNodeWallet.fromSeed(getBytes("0x" + seed))
}

export async function expectedXpub({ epub, platpair } = {}) {
    if (!epub || !platpair) throw new Error("invalidXpubValidation")
    const secret = await zen.secret(epub, platpair)
    return (await rootFromSecret(secret)).neuter().extendedKey
}

export async function resolvePublishedXpub({ runtime = zen, gun, party, field = "xpub", platpair = null } = {}) {
    if (!party?.pub) throw new Error("partyPubRequired")
    runtime = runtime || gun
    const xpub = await readPublishedValue({ runtime, pub: party.pub, field })
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

async function ensureUserNamespace({ runtime = zen, gun, pub, pair } = {}) {
    runtime = runtime || gun
    await new Promise((resolve, reject) => {
        runtime.get("~" + pub).get("pub").put(pub, (ack) => {
            if (ack?.err) return reject(new Error(ack.err))
            resolve()
        }, { authenticator: pair })
    })
}
