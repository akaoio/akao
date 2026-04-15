/**
 * Wallet label persistence via Gun.
 * No dependencies on /core/ absolute imports — safe to use in Node test runner.
 *
 * Storage path: gun.get(`~${pair.pub}`).get("wallets").put({ [id]: name })
 * Uses the same authenticated write pattern as Access.js.
 */

export async function writeLabel(pair, id, label) {
    const gun = globalThis.gun
    if (!gun || !pair) throw new Error("No gun/pair")
    return new Promise((resolve, reject) => {
        gun.get(`~${pair.pub}`).get("wallets").put(
            { [id]: String(label) },
            (ack) => { if (ack?.err) reject(new Error(ack.err)); else resolve() },
            { opt: { authenticator: pair } }
        )
    })
}

export async function readLabel(pub, id) {
    const gun = globalThis.gun
    if (!gun) return ""
    return new Promise((resolve) => {
        let done = false
        const finish = (val) => { if (!done) { done = true; resolve(val) } }
        gun.get(`~${pub}`).get("wallets").once((data) => {
            const raw = data?.[id]
            finish(typeof raw === "string" ? raw : "")
        })
        setTimeout(() => finish(""), 1000)
    })
}

export function subscribeLabel(pub, id, cb) {
    const gun = globalThis.gun
    if (!gun) return () => {}
    const node = gun.get(`~${pub}`).get("wallets")
    let active = true
    node.on((data) => {
        if (!active) return
        const raw = data?.[id]
        cb(typeof raw === "string" ? raw : "")
    })
    return () => { active = false; node.off() }
}
