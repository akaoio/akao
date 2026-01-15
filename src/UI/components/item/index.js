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
        this.shadowRoot.querySelector("a[is='ui-a']").dataset.to = `/item/${this.dataset.key}`
        this.subscriptions.push(Context.on("locale", this.render), this.states.on("name", [name, "textContent"]), this.states.on("description", [description, "textContent"]), this.states.on("price", [price, "textContent"]), this.states.on("sale", [sale, "textContent"]))
        const data = await DB.get(["statics", "items", this.dataset.key, "meta.json"])
        if (data) this.states.set(data)
        if (!this.states.has(["name", "price"])) this.render()
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }

    async render() {
        const data = await DB.get(["statics", "items", this.dataset.key, `${Context.get("locale").code}.json`])
        this.states.set(data)
    }
}

customElements.define("ui-item", ITEM)

export default ITEM
