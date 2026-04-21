/**
 * Wallet label persistence via ZEN graph.
 * No dependencies on /core/ absolute imports — safe to use in Node test runner.
 *
 * Storage path: zen.get(`~${pair.pub}`).get("wallets").put({ [id]: name })
 * Uses the same authenticated write pattern as Access.js.
 * zen is passed as the first argument to each function.
 */

export async function writeLabel(zen, pair, id, label) {
    if (!zen || !pair) throw new Error("No zen/pair")
    return new Promise((resolve, reject) => {
        zen.get(`~${pair.pub}`).get("wallets").put(
            { [id]: String(label) },
            (ack) => { if (ack?.err) reject(new Error(ack.err)); else resolve() },
            { authenticator: pair }
        )
    })
}

export async function readLabel(zen, pub, id) {
    if (!zen) return ""
    return new Promise((resolve) => {
        let done = false
        const finish = (val) => { if (!done) { done = true; resolve(val) } }
        zen.get(`~${pub}`).get("wallets").once((data) => {
            const raw = data?.[id]
            finish(typeof raw === "string" ? raw : "")
        })
        setTimeout(() => finish(""), 1000)
    })
}

export function subscribeLabel(zen, pub, id, cb) {
    if (!zen) return () => {}
    const node = zen.get(`~${pub}`).get("wallets")
    let active = true
    node.on((data) => {
        if (!active) return
        const raw = data?.[id]
        cb(typeof raw === "string" ? raw : "")
    })
    return () => { active = false; node.off() }
}
