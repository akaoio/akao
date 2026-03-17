import template from "./template.js"
import DB from "/core/DB.js"
import Router from "/core/Router.js"
import { Context } from "/core/Context.js"
import States from "/core/States.js"
import ITEM from "/UI/components/item/index.js"
import { render } from "/core/UI.js"

export class TAG extends HTMLElement {
    constructor() {
        super()
        this.states = new States({
            id: Context.get("params")?.tag || globalThis.history.state?.params?.tag,
            pages: 0
        })
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
        this.render = this.render.bind(this)
    }

    async connectedCallback() {
        this.subscriptions.push(Context.on("locale", this.render))
        await this.render()
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }

    async render() {
        const id = this.states.get("id")
        if (!id) return
        const locale = Context.get("locale")?.code
        if (!locale) return

        const meta = await DB.get(["statics", "tags", id, "meta.json"])
        if (!meta) return

        const pages = Math.max(1, parseInt(meta.pages || 1))
        this.states.set({ pages })

        const items = []
        for (let page = 1; page <= pages; page++) {
            const data = await DB.get(["statics", "tags", id, `${page}.json`])
            if (Array.isArray(data)) items.push(...data)
        }

        const children = items.map((item) => {
            const element = new ITEM()
            element.dataset.key = item
            return element
        })

        this.shadowRoot.querySelector("#title").textContent = id
        this.shadowRoot.querySelector("#items").replaceChildren(...children)

        Router.setHead({
            title: `${Context.get(["dictionary", "tags"]) || "Tags"}: ${id}`,
            description: ""
        })
    }
}

customElements.define("route-tag", TAG)

export default TAG