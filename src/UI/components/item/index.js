import template from "./template.js"
import { Context } from "/core/Context.js"
import States from "/core/States.js"
import { render } from "/core/UI.js"
import "/UI/components/game-item/index.js"
import logic from "./logic.js"

export class ITEM extends HTMLElement {
    constructor() {
        super()
        this.states = new States()
        this.attachShadow({ mode: "open" })
        this.subscriptions = []
        this.render = this.render.bind(this)
    }

    async connectedCallback() {
        if ("item" in this.dataset) {
            const item = JSON.parse(this.dataset.item || "{}")
            const catalog = item.catalog || (item.game ? "game" : null)
            if (catalog === "game") {
                this.shadowRoot.innerHTML = `<style>:host{display:contents}</style>`
                const el = document.createElement("ui-game-item")
                el.dataset.item = this.dataset.item
                this.shadowRoot.appendChild(el)
            }
            // future: else if (catalog === "physical") { ... }
            return
        }

        render(template, this.shadowRoot)

        const name = this.shadowRoot.querySelector("#name")
        const description = this.shadowRoot.querySelector("#description")
        const price = this.shadowRoot.querySelector("#price")
        const sale = this.shadowRoot.querySelector("#sale")

        // Support 2-tier keys ("game-id/item-id") and flat keys ("item-id")
        const key = this.dataset.key
        const { route: routePath } = logic.path(key)
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
        const data = await logic.meta(key)
        if (data) this.states.set(data)
        if (!this.states.has(["name", "price"])) this.render()
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }

    async render() {
        const key = this.dataset.key
        const data = await logic.locale(key, Context.get("locale").code)
        this.states.set(data)
    }
}

customElements.define("ui-item", ITEM)

export default ITEM
