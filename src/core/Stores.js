import IDB from "./IDB.js"
export const Indexes = {
    Hashes: new IDB({ name: "Hashes" }),
    Statics: new IDB({ name: "Statics" }),
    Auth: new IDB({ name: "Auth" }),
    Wallet: new IDB({ name: "Wallet" }),
    Lives: new IDB({ name: "Lives" }),
    Cart: new IDB({ name: "Cart" })
}

globalThis.Statics = globalThis.Statics || {}
export const Statics = globalThis.Statics

export const Wallets = {}
export const Elements = {}
