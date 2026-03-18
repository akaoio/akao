import template from "./template.js"
import DB from "/core/DB.js"
import Router from "/core/Router.js"
import { Context } from "/core/Context.js"
import States from "/core/States.js"
import ITEM from "/UI/components/item/index.js"
import { html, render } from "/core/UI.js"

export class GAME extends HTMLElement {
    constructor() {
        super()
        this.states = new States({
            id: Context.get("params")?.game || globalThis.history.state?.params?.game
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

        const [meta, data] = await Promise.all([
            DB.get(["statics", "games", id, "meta.json"]),
            DB.get(["statics", "games", id, `${locale}.json`])
        ])
        if (!meta || !data) return

        const dict = Context.get("dictionary") || {}

        this.shadowRoot.querySelector("#name").textContent = meta.name
        this.shadowRoot.querySelector("#description").textContent = data.description || ""

        const metaSection = this.shadowRoot.querySelector("#meta")
        const rows = []
        if (meta.genres?.length) rows.push(html`
            <div><strong>${dict.genres || "Genres"}</strong>:
                ${meta.genres.map(g => html`<span>${dict[g] || g}</span>`)}
            </div>`)
        if (meta.modes?.length) rows.push(html`
            <div><strong>${dict.modes || "Modes"}</strong>:
                ${meta.modes.map(m => html`<span>${dict[m] || m}</span>`)}
            </div>`)
        if (meta.platforms?.length) rows.push(html`
            <div><strong>${dict.platforms || "Platforms"}</strong>:
                ${meta.platforms.map(p => html`<span>${p}</span>`)}
            </div>`)
        render(rows, metaSection)

        const itemsMeta = await DB.get(["statics", "games", id, "items", "meta.json"])
        if (itemsMeta?.pages) {
            const slugs = []
            for (let page = 1; page <= itemsMeta.pages; page++) {
                const pageData = await DB.get(["statics", "games", id, "items", `${page}.json`])
                if (Array.isArray(pageData)) slugs.push(...pageData)
            }
            const children = slugs.map((slug) => {
                const el = new ITEM()
                el.dataset.key = slug
                return el
            })
            this.shadowRoot.querySelector("#items").replaceChildren(...children)
        }

        Router.setHead({
            title: meta.name,
            description: data.description || ""
        })
    }
}

customElements.define("route-game", GAME)

export default GAME
