import template from "./template.js"
import DB from "/core/DB.js"
import Router from "/core/Router.js"
import { Context } from "/core/Context.js"
import States from "/core/States.js"
import GAME_ITEM from "/UI/components/game-item/index.js"
import { html, render } from "/core/UI.js"

const SORT_OPTIONS = [
    { key: "name", label: "Name A→Z" },
    { key: "name-desc", label: "Name Z→A" },
    { key: "value-asc", label: "Price ↑" },
    { key: "value-desc", label: "Price ↓" }
]

function sortItems(items, sort) {
    const copy = [...items]
    if (sort === "name") return copy.sort((a, b) => (a.name || "").localeCompare(b.name || ""))
    if (sort === "name-desc") return copy.sort((a, b) => (b.name || "").localeCompare(a.name || ""))
    if (sort === "value-asc") return copy.sort((a, b) => (a.value || 0) - (b.value || 0))
    if (sort === "value-desc") return copy.sort((a, b) => (b.value || 0) - (a.value || 0))
    return copy
}

export class GAME extends HTMLElement {
    constructor() {
        super()
        this.states = new States({
            id: Context.get("params")?.game || globalThis.history.state?.params?.game,
            allItems: [],
            loadedPages: 0,
            totalPages: 0,
            totalItems: 0,
            activeType: null,
            activeRarity: null,
            sort: "name"
        })
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
        this.render = this.render.bind(this)
        this.applyFilters = this.applyFilters.bind(this)
        this.loadMore = this.loadMore.bind(this)
    }

    async connectedCallback() {
        this.subscriptions.push(Context.on("locale", this.render))
        await this.render()
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }

    applyFilters() {
        const allItems = this.states.get("allItems")
        const activeType = this.states.get("activeType")
        const activeRarity = this.states.get("activeRarity")
        const sort = this.states.get("sort")
        const loadedPages = this.states.get("loadedPages")
        const totalPages = this.states.get("totalPages")
        const totalItems = this.states.get("totalItems")

        let filtered = allItems
        if (activeType) filtered = filtered.filter((i) => i.type === activeType)
        if (activeRarity) filtered = filtered.filter((i) => i.rarity === activeRarity)
        filtered = sortItems(filtered, sort)

        // Count
        const countEl = this.shadowRoot.querySelector("#count")
        if (countEl) countEl.textContent = `${filtered.length} items`

        // Grid
        const grid = this.shadowRoot.querySelector("#items")
        const elements = filtered.map((item) => {
            const el = new GAME_ITEM()
            el.dataset.item = JSON.stringify(item)
            return el
        })
        grid.replaceChildren(...elements)

        // Load more button
        const loadMoreBtn = this.shadowRoot.querySelector("#load-more")
        if (loadMoreBtn) {
            const hasMore = loadedPages < totalPages
            loadMoreBtn.hidden = !hasMore
            if (hasMore) {
                const remaining = totalItems - allItems.length
                loadMoreBtn.textContent = `Load More — ${remaining} remaining`
            }
        }

        // Sync active filter states
        this.shadowRoot.querySelectorAll(".type-tabs button").forEach((btn) => {
            btn.classList.toggle("active", btn.dataset.type === (activeType || ""))
        })
        this.shadowRoot.querySelectorAll(".rarity-pills button").forEach((btn) => {
            btn.classList.toggle("active", btn.dataset.rarity === (activeRarity || ""))
        })
        this.shadowRoot.querySelectorAll(".sort-bar button").forEach((btn) => {
            btn.classList.toggle("active", btn.dataset.sort === sort)
        })
    }

    async loadMore() {
        const id = this.states.get("id")
        const loadedPages = this.states.get("loadedPages")
        const totalPages = this.states.get("totalPages")
        if (loadedPages >= totalPages) return

        const loadMoreBtn = this.shadowRoot.querySelector("#load-more")
        if (loadMoreBtn) loadMoreBtn.disabled = true

        const nextPage = loadedPages + 1
        const data = await DB.get(["statics", "games", id, "catalog", `${nextPage}.json`])
        if (data && Array.isArray(data)) {
            const allItems = [...this.states.get("allItems"), ...data]
            this.states.set({ allItems, loadedPages: nextPage })
        }

        if (loadMoreBtn) loadMoreBtn.disabled = false
        this.applyFilters()
    }

    async render() {
        const id = this.states.get("id")
        if (!id) return
        const locale = Context.get("locale")?.code
        if (!locale) return

        const [meta, data] = await Promise.all([DB.get(["statics", "games", id, "meta.json"]), DB.get(["statics", "games", id, `${locale}.json`])])
        if (!meta || !data) return

        // Hero
        this.shadowRoot.querySelector("#name").textContent = meta.name
        this.shadowRoot.querySelector("#description").textContent = data.description || ""

        const eyebrow = this.shadowRoot.querySelector("#eyebrow")
        const eyebrowPills = [...(meta.genres || []), ...(meta.modes || []), ...(meta.platforms || [])].map(
            (label) => html`
                <span class="game-hero__pill">${label}</span>
            `
        )
        render(eyebrowPills, eyebrow)

        const metaSection = this.shadowRoot.querySelector("#meta")
        const dict = Context.get("dictionary") || {}
        const metaRows = []
        if (meta.developer)
            metaRows.push(html`
                <span>
                    <strong>${dict.developer || "Dev"}</strong>
                    ${meta.developer}
                </span>
            `)
        if (meta.release)
            metaRows.push(html`
                <span>
                    <strong>${dict.release || "Released"}</strong>
                    ${meta.release}
                </span>
            `)
        render(metaRows, metaSection)

        Router.setHead({ title: meta.name, description: data.description || "" })

        // Load catalog meta
        const catalogMeta = await DB.get(["statics", "games", id, "catalog", "meta.json"])
        if (!catalogMeta) return

        const { pages = 1, types = [], rarities = [] } = catalogMeta
        this.states.set({ totalPages: pages, totalItems: catalogMeta.children || 0 })

        // Load first page only
        const firstPage = await DB.get(["statics", "games", id, "catalog", "1.json"])
        this.states.set({ allItems: Array.isArray(firstPage) ? firstPage : [], loadedPages: 1 })

        // Build type tabs
        const typeTabs = this.shadowRoot.querySelector("#type-tabs")
        const typeButtons = [this._makeFilterBtn("All", null, "activeType"), ...types.map((t) => this._makeFilterBtn(t, t, "activeType"))]
        typeTabs.replaceChildren(...typeButtons)

        // Build rarity pills
        const rarityPills = this.shadowRoot.querySelector("#rarity-pills")
        const rarityButtons = [
            this._makeFilterBtn("All", null, "activeRarity"),
            ...rarities.map((r) => {
                const btn = this._makeFilterBtn(r, r, "activeRarity")
                btn.dataset.rarity = r
                btn.setAttribute("data-rarity-key", r.toLowerCase())
                return btn
            })
        ]
        rarityPills.replaceChildren(...rarityButtons)

        // Build sort bar
        const sortBar = this.shadowRoot.querySelector("#sort")
        const sortButtons = SORT_OPTIONS.map((opt) => {
            const btn = document.createElement("button")
            btn.textContent = opt.label
            btn.dataset.sort = opt.key
            btn.addEventListener("click", () => {
                this.states.set({ sort: opt.key })
                this.applyFilters()
            })
            return btn
        })
        sortBar.replaceChildren(...sortButtons)

        // Wire load-more button
        const loadMoreBtn = this.shadowRoot.querySelector("#load-more")
        if (loadMoreBtn) {
            loadMoreBtn.replaceWith(loadMoreBtn.cloneNode(true))
            this.shadowRoot.querySelector("#load-more").addEventListener("click", this.loadMore)
        }

        this.applyFilters()
    }

    _makeFilterBtn(label, value, stateKey) {
        const btn = document.createElement("button")
        btn.textContent = label
        if (stateKey === "activeType") btn.dataset.type = value || ""
        if (stateKey === "activeRarity") btn.dataset.rarity = value || ""
        btn.addEventListener("click", () => {
            this.states.set({ [stateKey]: value })
            this.applyFilters()
        })
        return btn
    }
}

customElements.define("route-game", GAME)

export default GAME
