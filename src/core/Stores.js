import IDB from "./IDB.js"
export const Indexes = {
    Hashes: new IDB({ name: "Hashes" }),
    Statics: new IDB({ name: "Statics" }),
    Auth: new IDB({ name: "Auth" }),
    Lives: new IDB({ name: "Lives" }),
    Cart: new IDB({ name: "Cart" })
}

globalThis.Statics = globalThis.Statics || {}
export const Statics = globalThis.Statics

globalThis.Lives = globalThis.Lives || {}
export const Lives = globalThis.Lives

export const Chains = {}
export const Dexs = {}
export const Wallets = {}
export const Elements = {}