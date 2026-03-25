import template from "./template.js"
import DB from "/core/DB.js"
import Router from "/core/Router.js"
import { Context } from "/core/Context.js"
import States from "/core/States.js"
import GAME_ITEM from "/UI/components/game-item/index.js"
import { html, render } from "/core/UI.js"

const SORT_OPTIONS = [
    { key: "name", label: "Name", asc: "name", desc: "name-desc", indAsc: "↑", indDesc: "↓" },
    { key: "rarity", label: "Rarity", asc: "rarity-asc", desc: "rarity-desc", indAsc: "↑", indDesc: "↓" },
    { key: "price", label: "Price", asc: "value-asc", desc: "value-desc", indAsc: "↑", indDesc: "↓" }
]

function sortItems(items, sort, rarityOrder = []) {
    const copy = [...items]
    if (sort === "name") return copy.sort((a, b) => (a.name || "").localeCompare(b.name || ""))
    if (sort === "name-desc") return copy.sort((a, b) => (b.name || "").localeCompare(a.name || ""))
    if (sort === "value-asc") return copy.sort((a, b) => (a.value || 0) - (b.value || 0))
    if (sort === "value-desc") return copy.sort((a, b) => (b.value || 0) - (a.value || 0))
    if (sort === "rarity-asc" || sort === "rarity-desc") {
        const orderMap = Object.fromEntries(rarityOrder.map((r, i) => [r.toLowerCase(), i]))
        const rank = (item) => orderMap[(item.rarity || "").toLowerCase()] ?? rarityOrder.length
        copy.sort((a, b) => rank(a) - rank(b))
        if (sort === "rarity-desc") copy.reverse()
        return copy
    }
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
            sort: "name",
            search: "",
            rarityOrder: []
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
        ;["--game-primary", "--game-text-color", "--game-title-shadow"].forEach((v) =>
            document.documentElement.style.removeProperty(v)
        )
    }

    applyFilters() {
        const allItems = this.states.get("allItems")
        const activeType = this.states.get("activeType")
        const activeRarity = this.states.get("activeRarity")
        const sort = this.states.get("sort")
        const search = this.states.get("search")
        const rarityOrder = this.states.get("rarityOrder")
        const loadedPages = this.states.get("loadedPages")
        const totalPages = this.states.get("totalPages")
        const totalItems = this.states.get("totalItems")

        let filtered = allItems
        if (activeType) filtered = filtered.filter((i) => i.type === activeType)
        if (activeRarity) filtered = filtered.filter((i) => i.rarity === activeRarity)
        if (search) {
            const q = search.toLowerCase()
            filtered = filtered.filter((i) => (i.name || "").toLowerCase().includes(q))
        }
        filtered = sortItems(filtered, sort, rarityOrder)

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

        // Sync active filter states — buttons
        this.shadowRoot.querySelectorAll(".type-tabs button").forEach((btn) => {
            btn.classList.toggle("active", btn.dataset.type === (activeType || ""))
        })
        this.shadowRoot.querySelectorAll(".rarity-pills button").forEach((btn) => {
            btn.classList.toggle("active", btn.dataset.rarity === (activeRarity || ""))
        })

        // Sync active filter states — selects (mobile)
        const typeSelect = this.shadowRoot.querySelector("#type-select")
        if (typeSelect) {
            typeSelect.value = activeType || ""
            typeSelect.classList.toggle("active", !!activeType)
        }
        const raritySelect = this.shadowRoot.querySelector("#rarity-select")
        if (raritySelect) {
            raritySelect.value = activeRarity || ""
            raritySelect.classList.toggle("active", !!activeRarity)
            const rarityWrap = raritySelect.parentElement
            if (activeRarity) {
                const key = activeRarity.toLowerCase().replace(/\s+/g, "-")
                rarityWrap.style.setProperty("--select-accent", `var(--rarity-${key}, var(--neon-c))`)
            } else rarityWrap.style.removeProperty("--select-accent")
        }
        this.shadowRoot.querySelectorAll(".sort-bar button[data-sort-key]").forEach((btn) => {
            const isAsc = sort === btn.dataset.sortAsc
            const isDesc = sort === btn.dataset.sortDesc
            btn.classList.toggle("active", isAsc || isDesc)
            const dirEl = btn.querySelector(".sort-dir")
            if (dirEl) dirEl.textContent = isAsc ? btn.dataset.indAsc : isDesc ? btn.dataset.indDesc : ""
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
        this.states.set({ rarityOrder: meta.rarity_order || [] })

        // Inject rarity palette + game primary color as scoped CSS vars on :host
        if (meta.rarity_palette || meta.color) {
            let styleEl = this.shadowRoot.querySelector("#rarity-palette")
            if (!styleEl) {
                styleEl = document.createElement("style")
                styleEl.id = "rarity-palette"
                this.shadowRoot.appendChild(styleEl)
            }
            const vars = [
                ...Object.entries(meta.rarity_palette || {}).map(([key, val]) => `--rarity-${key}: ${val};`),
                ...(meta.color ? [`--game-primary: ${meta.color};`] : []),
                ...(meta.text_color ? [`--game-text-color: ${meta.text_color};`] : [])
            ].join(" ")
            styleEl.textContent = `:host { ${vars} }`
        }

        // Promote game vars to :root so sibling components (e.g. game-nav) can inherit them
        const gameVars = ["--game-primary", "--game-text-color", "--game-title-shadow"]
        const toRgba = (hex, a) => {
            const [r, g, b] = [1, 3, 5].map((i) => parseInt(hex.slice(i, i + 2), 16))
            return `rgba(${r},${g},${b},${a})`
        }

        if (meta.color) {
            document.documentElement.style.setProperty("--game-primary", meta.color)
            if (meta.text_color) document.documentElement.style.setProperty("--game-text-color", meta.text_color)
            else document.documentElement.style.removeProperty("--game-text-color")

            // --game-title-shadow: use meta.colors (multi-stop) if defined, else single color
            const glowColors = Array.isArray(meta.colors) && meta.colors.length ? meta.colors : [meta.color]
            const titleShadow = glowColors
                .flatMap((c, i) => [
                    `0 0 ${15 + i * 18}px ${toRgba(c, 0.65 - i * 0.08)}`,
                    `0 0 ${40 + i * 27}px ${toRgba(c, 0.3 - i * 0.05)}`
                ])
                .join(", ")
            document.documentElement.style.setProperty("--game-title-shadow", titleShadow)
        } else gameVars.forEach((v) => document.documentElement.style.removeProperty(v))

        // Load catalog meta
        const catalogMeta = await DB.get(["statics", "games", id, "catalog", "meta.json"])
        if (!catalogMeta) return

        const { pages = 1, types = [], rarities = [] } = catalogMeta
        this.states.set({ totalPages: pages, totalItems: catalogMeta.children || 0 })

        // Load first page only
        const firstPage = await DB.get(["statics", "games", id, "catalog", "1.json"])
        this.states.set({ allItems: Array.isArray(firstPage) ? firstPage : [], loadedPages: 1 })

        // Build type tabs with collapse
        const VISIBLE_TYPES = 6
        const typeTabs = this.shadowRoot.querySelector("#type-tabs")
        typeTabs.classList.remove("expanded")
        const typeButtons = [this._makeFilterBtn("All", null, "activeType"), ...types.map((t) => this._makeFilterBtn(t, t, "activeType"))]
        const overflowCount = Math.max(0, types.length - VISIBLE_TYPES)
        if (overflowCount > 0) {
            typeButtons.forEach((btn, i) => {
                if (i > VISIBLE_TYPES) btn.dataset.overflow = "true"
            })
            const toggleBtn = document.createElement("button")
            toggleBtn.className = "type-tabs__toggle"
            toggleBtn.textContent = `+${overflowCount} more`
            toggleBtn.addEventListener("click", () => {
                const expanded = typeTabs.classList.toggle("expanded")
                toggleBtn.textContent = expanded ? "Show less" : `+${overflowCount} more`
            })
            typeButtons.push(toggleBtn)
        }
        typeTabs.replaceChildren(...typeButtons)

        // Build type select (mobile)
        const typeSelect = this.shadowRoot.querySelector("#type-select")
        const typeSelectOptions = [{ label: "All Types", value: "" }, ...types.map((t) => ({ label: t, value: t }))]
        typeSelect.replaceChildren(
            ...typeSelectOptions.map(({ label, value }) => {
                const opt = document.createElement("option")
                opt.value = value
                opt.textContent = label
                return opt
            })
        )
        typeSelect.value = this.states.get("activeType") || ""
        typeSelect.onchange = () => {
            this.states.set({ activeType: typeSelect.value || null })
            this.applyFilters()
        }

        // Build rarity pills
        const rarityPills = this.shadowRoot.querySelector("#rarity-pills")
        const rarityButtons = [
            this._makeFilterBtn("All", null, "activeRarity"),
            ...rarities.map((r) => {
                const btn = this._makeFilterBtn(r, r, "activeRarity")
                btn.dataset.rarity = r
                const key = r.toLowerCase().replace(/\s+/g, "-")
                btn.setAttribute("data-rarity-key", key)
                btn.style.setProperty("--rarity-pill-color", `var(--rarity-${key}, var(--color-accent))`)
                return btn
            })
        ]
        rarityPills.replaceChildren(...rarityButtons)

        // Build rarity select (mobile)
        const raritySelect = this.shadowRoot.querySelector("#rarity-select")
        const raritySelectOptions = [{ label: "All Rarities", value: "" }, ...rarities.map((r) => ({ label: r, value: r }))]
        raritySelect.replaceChildren(
            ...raritySelectOptions.map(({ label, value }) => {
                const opt = document.createElement("option")
                opt.value = value
                opt.textContent = label
                return opt
            })
        )
        raritySelect.value = this.states.get("activeRarity") || ""
        raritySelect.onchange = () => {
            this.states.set({ activeRarity: raritySelect.value || null })
            this.applyFilters()
        }

        // Wire search input + autocomplete
        const searchInput = this.shadowRoot.querySelector("#search")
        const suggestions = this.shadowRoot.querySelector("#search-suggestions")
        searchInput.value = ""
        this.states.set({ search: "" })

        const closeSuggestions = () => suggestions.classList.remove("open")

        const openSuggestions = (query) => {
            if (!query || query.length < 2) return closeSuggestions()
            const q = query.toLowerCase()
            const matches = this.states
                .get("allItems")
                .filter((i) => (i.name || "").toLowerCase().includes(q))
                .slice(0, 8)
            if (!matches.length) return closeSuggestions()

            const items = matches.map((item) => {
                const li = document.createElement("li")
                li.className = "catalog-search__suggestion"

                const nameEl = document.createElement("span")
                nameEl.className = "suggestion__name"
                nameEl.textContent = item.name || ""

                const rarityEl = document.createElement("span")
                rarityEl.className = "suggestion__rarity"
                rarityEl.textContent = item.rarity || ""
                const key = (item.rarity || "").toLowerCase().replace(/\s+/g, "-")
                rarityEl.style.setProperty("--rarity-pill-color", `var(--rarity-${key}, var(--color-accent))`)

                li.append(nameEl, rarityEl)
                li.addEventListener("mousedown", (e) => {
                    e.preventDefault()
                    searchInput.value = item.name
                    this.states.set({ search: item.name })
                    closeSuggestions()
                    this.applyFilters()
                })
                return li
            })

            suggestions.replaceChildren(...items)
            suggestions.classList.add("open")
        }

        searchInput.oninput = (e) => {
            const val = e.target.value.trim()
            this.states.set({ search: val })
            openSuggestions(val)
            this.applyFilters()
        }

        searchInput.onfocus = (e) => openSuggestions(e.target.value.trim())

        searchInput.onblur = () => setTimeout(closeSuggestions, 150)

        searchInput.onkeydown = (e) => {
            const lis = [...suggestions.querySelectorAll(".catalog-search__suggestion")]
            const hi = lis.findIndex((li) => li.classList.contains("highlighted"))
            if (e.key === "ArrowDown") {
                e.preventDefault()
                lis[hi]?.classList.remove("highlighted")
                lis[Math.min(hi + 1, lis.length - 1)]?.classList.add("highlighted")
            } else if (e.key === "ArrowUp") {
                e.preventDefault()
                lis[hi]?.classList.remove("highlighted")
                lis[Math.max(hi - 1, 0)]?.classList.add("highlighted")
            } else if (e.key === "Enter" && hi >= 0) {
                e.preventDefault()
                lis[hi]?.dispatchEvent(new MouseEvent("mousedown"))
            } else if (e.key === "Escape") {
                closeSuggestions()
                searchInput.blur()
            }
        }

        // Build sort bar
        const sortBar = this.shadowRoot.querySelector("#sort")
        const sortButtons = SORT_OPTIONS.map((opt) => {
            const btn = document.createElement("button")
            btn.dataset.sortKey = opt.key
            btn.dataset.sortAsc = opt.asc
            btn.dataset.sortDesc = opt.desc
            btn.dataset.indAsc = opt.indAsc
            btn.dataset.indDesc = opt.indDesc

            const labelEl = document.createElement("span")
            labelEl.textContent = opt.label
            const dirEl = document.createElement("span")
            dirEl.className = "sort-dir"
            btn.append(labelEl, dirEl)

            btn.addEventListener("click", () => {
                const current = this.states.get("sort")
                const next = current === opt.asc ? opt.desc : opt.asc
                this.states.set({ sort: next })
                this.applyFilters()
            })
            return btn
        })
        sortBar.replaceChildren(...sortButtons)

        // Lock every sort button to the same width so switching active option
        // never shifts the search input. Measure at the widest possible indicator
        // text ("A→Z"), then restore empty dir slots and pin min-width.
        const allDirs = [...sortBar.querySelectorAll(".sort-dir")]
        allDirs.forEach((d) => {
            d.textContent = "↑"
        })
        const maxBtnW = Math.max(...[...sortBar.children].map((b) => b.offsetWidth))
        allDirs.forEach((d) => {
            d.textContent = ""
        })
        sortBar.querySelectorAll("button").forEach((b) => {
            b.style.minWidth = `${maxBtnW}px`
        })

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
