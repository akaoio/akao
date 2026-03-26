/**
 * Cart tests — browser only (IndexedDB + static item metadata required).
 */
import Test from "../Test.js"
import { Cart } from "../Cart.js"
import { sha256 } from "../Utils.js"
import { Indexes } from "../Stores.js"

/** Create a fresh Cart with pre-seeded item metadata to avoid real DB calls. */
async function makeCart(items = {}) {
    await Indexes.Cart.get("cart").del()
    const cart = new Cart()
    // Pre-seed metas so add()/update() never fetches from DB
    Object.assign(cart.metas, items)
    await cart.ready
    return cart
}

const ITEM_A = { id: "green-tea", sku: "default", price: 10, name: "Green Tea" }
const ITEM_B = { id: "black-tea", sku: "large", price: 15, name: "Black Tea" }

const METAS = {
    "green-tea": { price: 10, name: "Green Tea" },
    "black-tea": { price: 15, name: "Black Tea" }
}

Test.describe("Cart — key generation", () => {

    Test.it("key is sha256 of id+sku with no variants", async () => {
        const expected = sha256("id:green-tea.sku:default.")
        const cart = await makeCart(METAS)
        await cart.add({ id: "green-tea", sku: "default" })
        const keys = Object.keys(cart.states.get("cart"))
        Test.assert.equal(keys.length, 1)
        Test.assert.equal(keys[0], expected)
    })

    Test.it("different sku produces different key", async () => {
        const cart = await makeCart(METAS)
        await cart.add({ id: "green-tea", sku: "small" })
        await cart.add({ id: "green-tea", sku: "large" })
        const keys = Object.keys(cart.states.get("cart"))
        Test.assert.equal(keys.length, 2)
    })

    Test.it("variant pairs are sorted before hashing (order-independent)", async () => {
        const cartA = await makeCart(METAS)
        const cartB = await makeCart(METAS)
        await cartA.add({ id: "green-tea", sku: "s", color: "red", size: "M" })
        await cartB.add({ id: "green-tea", sku: "s", size: "M", color: "red" })
        const keyA = Object.keys(cartA.states.get("cart"))[0]
        const keyB = Object.keys(cartB.states.get("cart"))[0]
        Test.assert.equal(keyA, keyB)
    })

}, { browser: true })

Test.describe("Cart — add()", () => {

    Test.it("adds item to cart", async () => {
        const cart = await makeCart(METAS)
        await cart.add({ id: "green-tea", sku: "default", quantity: 2 })
        const keys = Object.keys(cart.states.get("cart"))
        Test.assert.equal(keys.length, 1)
    })

    Test.it("adding same item twice accumulates quantity", async () => {
        const cart = await makeCart(METAS)
        await cart.add({ id: "green-tea", sku: "default", quantity: 1 })
        await cart.add({ id: "green-tea", sku: "default", quantity: 2 })
        const keys = Object.keys(cart.states.get("cart"))
        Test.assert.equal(keys.length, 1)
        Test.assert.equal(cart.states.get("cart")[keys[0]].quantity, 3)
    })

    Test.it("missing id or sku returns early without adding", async () => {
        const cart = await makeCart(METAS)
        await cart.add({ id: "green-tea" })
        await cart.add({ sku: "default" })
        const keys = Object.keys(cart.states.get("cart"))
        Test.assert.equal(keys.length, 0)
    })

    Test.it("quantity defaults to 1 when not provided", async () => {
        const cart = await makeCart(METAS)
        await cart.add({ id: "green-tea", sku: "default" })
        const keys = Object.keys(cart.states.get("cart"))
        Test.assert.equal(cart.states.get("cart")[keys[0]].quantity, 1)
    })

}, { browser: true })

Test.describe("Cart — remove() / increase() / decrease()", () => {

    Test.it("remove() deletes the item", async () => {
        const cart = await makeCart(METAS)
        await cart.add({ id: "green-tea", sku: "default" })
        const key = Object.keys(cart.states.get("cart"))[0]
        cart.remove(key)
        Test.assert.equal(Object.keys(cart.states.get("cart")).length, 0)
    })

    Test.it("increase() adds to quantity", async () => {
        const cart = await makeCart(METAS)
        await cart.add({ id: "green-tea", sku: "default", quantity: 1 })
        const key = Object.keys(cart.states.get("cart"))[0]
        cart.increase(key, 3)
        Test.assert.equal(cart.states.get("cart")[key].quantity, 4)
    })

    Test.it("decrease() subtracts from quantity (minimum 1)", async () => {
        const cart = await makeCart(METAS)
        await cart.add({ id: "green-tea", sku: "default", quantity: 3 })
        const key = Object.keys(cart.states.get("cart"))[0]
        cart.decrease(key, 2)
        Test.assert.equal(cart.states.get("cart")[key].quantity, 1)
        // Try to go below 1
        cart.decrease(key, 5)
        Test.assert.equal(cart.states.get("cart")[key].quantity, 1)
    })

    Test.it("remove() returns false for non-existent key", async () => {
        const cart = await makeCart(METAS)
        const result = cart.remove("nonexistent-key")
        Test.assert.falsy(result)
    })

}, { browser: true })

Test.describe("Cart — update() totals", () => {

    Test.it("correctly calculates total and quantity after add", async () => {
        const cart = await makeCart(METAS)
        await cart.add({ id: "green-tea", sku: "default", quantity: 2 })
        await cart.add({ id: "black-tea", sku: "large", quantity: 1 })
        await cart.update()
        Test.assert.equal(cart.states.get("total"), 35) // 10*2 + 15*1
        Test.assert.equal(cart.states.get("quantity"), 3)
    })

    Test.it("uses sale price when available", async () => {
        const cart = await makeCart({ "sale-item": { price: 20, sale: 12 } })
        await cart.add({ id: "sale-item", sku: "s", quantity: 1 })
        await cart.update()
        Test.assert.equal(cart.states.get("total"), 12)
    })

    Test.it("list is sorted by timestamp (newest first)", async () => {
        const cart = await makeCart(METAS)
        await cart.add({ id: "green-tea", sku: "default" })
        await new Promise(r => setTimeout(r, 5))
        await cart.add({ id: "black-tea", sku: "large" })
        await cart.update()
        const list = cart.states.get("list")
        Test.assert.equal(list.length, 2)
        Test.assert.truthy(list[0].timestamp >= list[1].timestamp)
    })

}, { browser: true })
