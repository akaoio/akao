import { Indexes } from "./Stores.js"
import States from "./States.js"
import { sha256 } from "./Utils.js"
import DB from "./DB.js"

export class Cart {
    constructor() {
        this.states = new States({ cart: {}, list: [], total: 0, quantity: 0 })
        this.metas = {}
        this.init = this.init.bind(this)
        this.add = this.add.bind(this)
        this.remove = this.remove.bind(this)
        this.update = this.update.bind(this)
        this.ready = this.init()
    }

    async init() {
        this.states.on("cart", this.update)
        const data = await Indexes.Cart.get("cart").once()
        if (data) this.states.set({ cart: data })
    }

    async add(data = {}) {
        let { id, sku, quantity, ...rest } = data
        if (!id || !sku) return
        const meta = this.metas?.[id] || (await DB.get(["statics", "items", id, "meta.json"]))
        if (meta && !this.metas?.[id]) this.metas[id] = meta
        const key = sha256(
            `id:${id}.sku:${sku}.${Object.entries(rest)
                .sort()
                .map(([k, v]) => `${k}:${v}`)
                .join(".")}`
        )
        quantity = Number(quantity) || 1
        const cart = this.states.get("cart") || {}
        const item = cart[key] ? { ...cart[key], quantity: cart[key].quantity + quantity, timestamp: Date.now() } : { id, sku, quantity, ...rest, timestamp: Date.now() }
        this.states.set({ cart: { ...cart, [key]: item } })
        return true
    }

    remove(key) {
        if (!key || !this.states.get(["cart", key])) return
        this.states.del(["cart", key])
        return true
    }

    increase(key, amount = 1) {
        if (!key || !this.states.get(["cart", key])) return
        const cart = this.states.get("cart")
        const item = cart[key]
        this.states.set({ cart: { ...cart, [key]: { ...item, quantity: Math.max(1, item.quantity + amount) } } })
        return true
    }
    decrease(key, amount = 1) {
        if (!key || !this.states.get(["cart", key])) return
        const cart = this.states.get("cart")
        const item = cart[key]
        this.states.set({ cart: { ...cart, [key]: { ...item, quantity: Math.max(1, item.quantity - amount) } } })
        return true
    }

    async update() {
        let total = 0
        let quantity = 0
        const list = []
        const sorted = Object.entries(this.states.get("cart")).sort((a, b) => b[1].timestamp - a[1].timestamp)
        for (const [key, item] of sorted) {
            const meta = this.metas?.[item.id] || (await DB.get(["statics", "items", item.id, "meta.json"]))
            if (!meta) return this.remove(key)
            if (!this.metas[item.id]) this.metas[item.id] = meta
            const $price = meta?.sale || meta?.price || 0
            const $total = $price * item.quantity
            total += $total
            quantity += item.quantity
            list.push({ key, ...item, price: $price, total: $total })
        }
        this.states.set({ list, total, quantity })
        Indexes.Cart.get("cart").put(this.states.get("cart"))
        return list
    }
}

export const cart = new Cart()

export default cart
