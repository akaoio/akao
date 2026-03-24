import template from "./template.js"
import DB from "/core/DB.js"
import { Context } from "/core/Context.js"
import States from "/core/States.js"
import { render } from "/core/UI.js"

export class ITEM extends HTMLElement {
    constructor() {
        super()
        this.states = new States()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
        this.render = this.render.bind(this)
    }

    async connectedCallback() {
        const name = this.shadowRoot.querySelector("#name")
        const description = this.shadowRoot.querySelector("#description")
        const price = this.shadowRoot.querySelector("#price")
        const sale = this.shadowRoot.querySelector("#sale")

        // Support 2-tier keys ("game-id/item-id") and flat keys ("item-id")
        const key = this.dataset.key
        const parts = key.includes("/") ? key.split("/") : null
        const dbMetaPath = parts
            ? ["statics", "items", ...parts, "meta.json"]
            : ["statics", "items", key, "meta.json"]
        const routePath = parts ? `/item/${parts.join("/")}` : `/item/${key}`

        this.shadowRoot.querySelector("a[is='ui-a']").dataset.to = routePath
        this.subscriptions.push(
            Context.on("locale", this.render),
            this.states.on("name", [name, "textContent"]),
            this.states.on("description", [description, "textContent"]),
            this.states.on("price", [price, "dataset", "amount"]),
            this.states.on("sale", [sale, "dataset", "amount"]),
            this.states.on("currency", ({ value }) => {
                price.dataset.base = value
                sale.dataset.base = value
            })
        )
        const data = await DB.get(dbMetaPath)
        if (data) this.states.set(data)
        if (!this.states.has(["name", "price"])) this.render()
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }

    async render() {
        const key = this.dataset.key
        const parts = key.includes("/") ? key.split("/") : null
        const dbLocalePath = parts
            ? ["statics", "items", ...parts, `${Context.get("locale").code}.json`]
            : ["statics", "items", key, `${Context.get("locale").code}.json`]
        const data = await DB.get(dbLocalePath)
        this.states.set(data)
    }
}

customElements.define("ui-item", ITEM)

export default ITEM
