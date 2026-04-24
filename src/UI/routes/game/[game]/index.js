import template from "./template.js"
import Router from "/core/Router.js"
import { Context } from "/core/Context.js"
import States from "/core/States.js"
import ITEM from "/UI/components/item/index.js"
import { html, render } from "/core/UI.js"
import logic from "./logic.js"

const INITIAL_PAGES = 3
const PAGE_SIZE = 30
const ITEM_CONCURRENCY = 10

// Fetch items in bounded batches to avoid ERR_INSUFFICIENT_RESOURCES from
// firing hundreds of concurrent requests (each item needs meta.json + locale.json).
async function loadItemsBatched(ids, gameId, locale) {
    const results = []
    for (let i = 0; i < ids.length; i += ITEM_CONCURRENCY) {
        const batch = ids.slice(i, i + ITEM_CONCURRENCY)
        const items = (await Promise.all(batch.map((itemId) => logic.item(gameId, itemId, locale)))).filter(Boolean)
        results.push(...items)
    }
    return results
}

const SORT_OPTIONS = [
    { key: "name", label: "Name", asc: "name", desc: "name-desc", indAsc: "↑", indDesc: "↓" },
    { key: "price", label: "Price", asc: "value-asc", desc: "value-desc", indAsc: "↑", indDesc: "↓" },
    { key: "rarity", label: "Rarity", asc: "rarity-asc", desc: "rarity-desc", indAsc: "↑", indDesc: "↓" }
]

export class GAME extends HTMLElement {
    static module = import.meta.url
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
        this._displayPage = 1
        this._readyPromise = null
        this._generation = 0
        this.subs = []
        this._loadingAll = false
        this.render = this.render.bind(this)
        this.applyFilters = this.applyFilters.bind(this)
        this._handleFilterChange = this._handleFilterChange.bind(this)
        this.loadMore = this.loadMore.bind(this)
    }

    async connectedCallback() {
        this.subs.push(Context.on("locale", this.render))
        await this.render()
        this._setupStickyPill()
    }

    _setupStickyPill() {
        const sticky = this.shadowRoot.querySelector(".catalog-sticky")
        const sentinel = this.shadowRoot.querySelector(".sticky-sentinel")
        const pill = this.shadowRoot.querySelector("#catalog-pill")
        if (!sticky || !sentinel) return

        const stickyTopPx = parseInt(getComputedStyle(sticky).top) || 0

        // Sets the inline pixel width that acts as the "from" value for transitions.
        // Called after any change that may resize the pill (stuck, filter change).
        const syncPillWidth = () => {
            if (!pill) return
            if (window.matchMedia("(min-width: 768px)").matches) return
            this._pillWidth = pill.getBoundingClientRect().width
            sticky.style.width = this._pillWidth + "px"
        }

        // Collapses to pill with an animated transition (used for explicit user actions).
        // Two-frame technique to get a CSS width transition from full-width → pill-width:
        //   Frame 1: pin the current px width as inline style (gives transition a "from"),
        //            remove is-expanded so CSS rules for the collapsed state apply.
        //   Between frames: browser resolves layout with fit-content on the pill.
        //   Frame 2: measure the natural pill width and write it as inline style ("to").
        //            The transition on .catalog-sticky then animates between the two.
        const animateCollapse = () => {
            // Frame 1 — lock in current rendered width and drop is-expanded.
            sticky.style.width = sticky.getBoundingClientRect().width + "px"
            sticky.classList.remove("is-expanded")
            // Frame 2 — measure the pill's natural fit-content width and set as target.
            requestAnimationFrame(() => {
                if (!pill) return
                this._pillWidth = pill.getBoundingClientRect().width
                sticky.style.width = this._pillWidth + "px"
            })
        }

        // Expands to full bar with an animated transition.
        // Browser sees: from _pillWidth px (inline) → to 100% (CSS .is-stuck.is-expanded).
        const expandSticky = () => {
            sticky.style.width = ""
            sticky.classList.add("is-expanded")
        }

        this._setupStickyObserver(sticky, sentinel, stickyTopPx, syncPillWidth)
        this._setupPillExpand(pill, expandSticky)
        this._setupPillSegments(expandSticky)
        this._setupCollapseControls(sticky, animateCollapse)

        // Store syncPillWidth so applyFilters can update the width when pill content changes.
        this._syncPillWidth = syncPillWidth
    }

    _setupStickyObserver(sticky, sentinel, stickyTopPx, syncPillWidth) {
        this._stickyObserver = new IntersectionObserver(
            ([entry]) => {
                if (!entry.isIntersecting) {
                    // Scroll-triggered stick: instant snap.
                    // Add class first, then measure synchronously — getBoundingClientRect()
                    // forces a style recalculation so .is-stuck CSS (including
                    // width: fit-content on .catalog-pill) is applied before we measure.
                    sticky.classList.add("is-stuck")
                    syncPillWidth()
                } else {
                    // Scroll-triggered unstick: instant snap back to full bar.
                    sticky.style.width = ""
                    sticky.classList.remove("is-stuck", "is-expanded")
                }
            },
            { rootMargin: `-${stickyTopPx}px 0px 0px 0px`, threshold: 0 }
        )
        this._stickyObserver.observe(sentinel)
        this.subs.push(() => this._stickyObserver.disconnect())
    }

    _setupPillExpand(pill, expandSticky) {
        if (!pill) return
        pill.addEventListener("click", expandSticky)
        this.subs.push(() => pill.removeEventListener("click", expandSticky))
    }

    // Individual pill segment actions — each stops propagation so the
    // general pill-expand doesn't also fire, then handles its own action.
    _setupPillSegments(expandSticky) {
        const pillSortEl = this.shadowRoot.querySelector("#pill-sort")
        if (pillSortEl) {
            const onPillSort = (e) => {
                e.stopPropagation()
                const sortCycle = SORT_OPTIONS.flatMap((o) => [o.asc, o.desc])
                const current = this.states.get("sort")
                const idx = sortCycle.indexOf(current)
                const next = sortCycle[(idx + 1) % sortCycle.length]
                this.states.set({ sort: next })
                this._displayPage = 1
                this.applyFilters()
            }
            pillSortEl.addEventListener("click", onPillSort)
            this.subs.push(() => pillSortEl.removeEventListener("click", onPillSort))
        }

        const pillTypeEl = this.shadowRoot.querySelector("#pill-type")
        if (pillTypeEl) {
            const onPillType = (e) => {
                e.stopPropagation()
                expandSticky()
                requestAnimationFrame(() => this.shadowRoot.querySelector("#type-filter")?.shadowRoot?.querySelector("#select")?.focus())
            }
            pillTypeEl.addEventListener("click", onPillType)
            this.subs.push(() => pillTypeEl.removeEventListener("click", onPillType))
        }

        const pillRarityDotEl = this.shadowRoot.querySelector("#pill-rarity-dot")
        if (pillRarityDotEl) {
            const onPillRarityDot = (e) => {
                e.stopPropagation()
                expandSticky()
                requestAnimationFrame(() => this.shadowRoot.querySelector("#rarity-filter")?.shadowRoot?.querySelector("#select")?.focus())
            }
            pillRarityDotEl.addEventListener("click", onPillRarityDot)
            pillRarityDotEl.addEventListener("keydown", (e) => {
                if (e.key === "Enter" || e.key === " ") {
                    e.preventDefault()
                    onPillRarityDot(e)
                }
            })
            this.subs.push(() => pillRarityDotEl.removeEventListener("click", onPillRarityDot))
        }

        const pillSearchEl = this.shadowRoot.querySelector("#pill-search")
        if (pillSearchEl) {
            const onPillSearch = (e) => {
                e.stopPropagation()
                expandSticky()
                requestAnimationFrame(() => this.shadowRoot.querySelector("#search")?.focus())
            }
            pillSearchEl.addEventListener("click", onPillSearch)
            this.subs.push(() => pillSearchEl.removeEventListener("click", onPillSearch))
        }
    }

    _setupCollapseControls(sticky, animateCollapse) {
        const collapseBtn = this.shadowRoot.querySelector("#catalog-collapse")
        if (collapseBtn) {
            const onCollapse = (e) => {
                e.stopPropagation()
                animateCollapse()
            }
            collapseBtn.addEventListener("click", onCollapse)
            this.subs.push(() => collapseBtn.removeEventListener("click", onCollapse))
        }

        // Pointer outside → collapse (animated).
        this._onPointerDown = (e) => {
            if (!sticky.classList.contains("is-expanded")) return
            if (!e.composedPath().includes(sticky)) animateCollapse()
        }
        document.addEventListener("pointerdown", this._onPointerDown)
        this.subs.push(() => document.removeEventListener("pointerdown", this._onPointerDown))
    }

    disconnectedCallback() {
        this.subs.forEach((off) => off())
        ;["--game-primary", "--game-text-color", "--game-title-shadow"].forEach((v) => document.documentElement.style.removeProperty(v))
    }

    async _loadAllInBackground(id, locale, fromPage, totalPages, gen) {
        const count = totalPages - fromPage
        const pageIdBatches = await Promise.all(Array.from({ length: count }, (_, i) => logic.page(id, fromPage + i + 1)))
        const allIds = pageIdBatches.flatMap((ids) => (Array.isArray(ids) ? ids : []))
        const newItems = await loadItemsBatched(allIds, id, locale)

        if (this._generation !== gen) return // render() was called again — discard stale data

        const allItems = [...this.states.get("allItems"), ...newItems]
        this.states.set({ allItems, loadedPages: totalPages })
        this._readyPromise = null
        this.applyFilters() // refresh Load More count now that all items are in memory
    }

    async _handleFilterChange() {
        this._displayPage = 1
        this.applyFilters() // sync filter UI state immediately (colors, active classes, count)
        if (this._readyPromise) {
            const wrap = this.shadowRoot.querySelector("#items-wrap")
            if (wrap) wrap.classList.add("is-loading-all")
            await this._readyPromise
            if (wrap) wrap.classList.remove("is-loading-all")
        }
        this.applyFilters()
    }

    applyFilters() {
        const allItems = this.states.get("allItems")
        const activeType = this.states.get("activeType")
        const activeRarity = this.states.get("activeRarity")
        const sort = this.states.get("sort")
        const search = this.states.get("search")
        const rarityOrder = this.states.get("rarityOrder")
        const totalItems = this.states.get("totalItems")

        let filtered = allItems
        if (activeType) filtered = filtered.filter((i) => i.type === activeType)
        if (activeRarity) filtered = filtered.filter((i) => (i.rarity || "").toLowerCase() === activeRarity.toLowerCase())
        if (search) {
            const q = search.toLowerCase()
            filtered = filtered.filter((i) => (i.name || "").toLowerCase().includes(q))
        }
        filtered = logic.sort(filtered, sort, rarityOrder)

        const anyFilterActive = !!(activeType || activeRarity || search)
        // While background pages are still loading, the filtered count is partial — show "…"
        const countValue = anyFilterActive ? (this._readyPromise ? `${filtered.length}+` : filtered.length) : totalItems

        // Count — when browsing without a filter, show the catalog total immediately
        // so the number doesn't flicker as background pages load in
        const searchEl = this.shadowRoot.querySelector("#search")
        if (searchEl) searchEl.setAttribute("count", countValue)

        // ── Sync collapsed pill ──
        const pillType = this.shadowRoot.querySelector("#pill-type")
        const pillRarityDot = this.shadowRoot.querySelector("#pill-rarity-dot")
        const pillCount = this.shadowRoot.querySelector("#pill-count-num")
        const pillSort = this.shadowRoot.querySelector("#pill-sort")
        const pillSearch = this.shadowRoot.querySelector("#pill-search")

        if (pillType) {
            pillType.textContent = activeType || "All"
            pillType.classList.add("active")
        }

        if (pillRarityDot)
            if (activeRarity) {
                const key = activeRarity.toLowerCase().replace(/\s+/g, "-")
                pillRarityDot.style.setProperty("--pill-rarity-color", `var(--rarity-${key}, var(--color-accent))`)
                pillRarityDot.classList.add("active")
            } else {
                pillRarityDot.style.removeProperty("--pill-rarity-color")
                pillRarityDot.classList.remove("active")
            }

        if (pillCount) pillCount.textContent = countValue
        if (pillSort) {
            const activeSortOpt = SORT_OPTIONS.find((o) => o.asc === sort || o.desc === sort)
            if (activeSortOpt) {
                const isDesc = sort === activeSortOpt.desc
                pillSort.textContent = `${activeSortOpt.label}${isDesc ? "↓" : "↑"}`
            }
        }
        if (pillSearch) pillSearch.classList.toggle("active", !!search)

        // Re-sync inline pill width if stuck and collapsed (pill content may have changed size).
        const stickyEl = this.shadowRoot.querySelector(".catalog-sticky")
        if (stickyEl && stickyEl.classList.contains("is-stuck") && !stickyEl.classList.contains("is-expanded") && this._syncPillWidth) this._syncPillWidth()

        // Grid — always paginate via the display window, filter or not
        const visible = filtered.slice(0, this._displayPage * PAGE_SIZE)
        const grid = this.shadowRoot.querySelector("#items")
        const elements = visible.map((item) => {
            const el = new ITEM()
            el.dataset.item = JSON.stringify(item)
            return el
        })
        grid.replaceChildren(...elements)

        // Empty state — only show when background loading is done and nothing matched
        const emptyEl = this.shadowRoot.querySelector("#items-empty")
        if (emptyEl) {
            const isEmpty = !this._readyPromise && filtered.length === 0
            emptyEl.hidden = !isEmpty
            if (isEmpty) {
                const dict = Context.get("dictionary") || {}
                emptyEl.textContent = dict.noItemsFound || "No items found"
            }
        }

        // Load More — client-side only, advances the display window over in-memory data
        const loadMoreBtn = this.shadowRoot.querySelector("#load-more")
        if (loadMoreBtn) {
            const hasMore = filtered.length > this._displayPage * PAGE_SIZE
            loadMoreBtn.hidden = !hasMore
            if (hasMore) {
                const remaining = filtered.length - this._displayPage * PAGE_SIZE
                loadMoreBtn.textContent = `Load More — ${remaining} remaining`
            }
        }

        // Sync filter components
        const typeFilter = this.shadowRoot.querySelector("#type-filter")
        if (typeFilter) typeFilter.value = activeType

        const rarityFilter = this.shadowRoot.querySelector("#rarity-filter")
        if (rarityFilter) rarityFilter.value = activeRarity
        this.shadowRoot.querySelectorAll(".sort-bar button[data-sort-key]").forEach((btn) => {
            const isAsc = sort === btn.dataset.sortAsc
            const isDesc = sort === btn.dataset.sortDesc
            btn.classList.toggle("active", isAsc || isDesc)
            const dirEl = btn.querySelector(".sort-dir")
            if (dirEl) dirEl.textContent = isAsc ? btn.dataset.indAsc : isDesc ? btn.dataset.indDesc : ""
            if (btn.dataset.sortKey === "rarity") btn.disabled = !!activeRarity
        })
    }

    loadMore() {
        this._displayPage++
        this.applyFilters()
    }

    async render() {
        const id = this.states.get("id")
        if (!id) return
        const locale = Context.get("locale")?.code
        if (!locale) return

        const [meta, data] = await Promise.all([logic.meta(id), logic.info(id, locale)])
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
        if (meta.rarity_palette || meta.color || meta.hero) {
            let styleEl = this.shadowRoot.querySelector("#rarity-palette")
            if (!styleEl) {
                styleEl = document.createElement("style")
                styleEl.id = "rarity-palette"
                this.shadowRoot.appendChild(styleEl)
            }
            const vars = [...Object.entries(meta.rarity_palette || {}).map(([key, val]) => `--rarity-${key}: ${val};`), ...(meta.color ? [`--game-primary: ${meta.color};`] : []), ...(meta.text_color ? [`--game-text-color: ${meta.text_color};`] : []), ...(meta.hero ? [`--game-hero-image: url("${meta.hero}");`] : [])].join(" ")
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
            const titleShadow = glowColors.flatMap((c, i) => [`0 0 ${15 + i * 18}px ${toRgba(c, 0.65 - i * 0.08)}`, `0 0 ${40 + i * 27}px ${toRgba(c, 0.3 - i * 0.05)}`]).join(", ")
            document.documentElement.style.setProperty("--game-title-shadow", titleShadow)
        } else gameVars.forEach((v) => document.documentElement.style.removeProperty(v))

        // Load items meta
        const itemsMeta = await logic.catalog(id)
        if (!itemsMeta) return

        const { pages = 1, types = [] } = itemsMeta
        const rarities = meta.rarity_order || []
        this.states.set({ totalPages: pages, totalItems: itemsMeta.children || 0 })

        // Reset display state for this render (e.g. locale change)
        this._generation++
        this._displayPage = 1
        this._readyPromise = null

        // Load first N pages in parallel for immediate display
        const initialPages = Math.min(INITIAL_PAGES, pages)
        const pageIdBatches = await Promise.all(Array.from({ length: initialPages }, (_, i) => logic.page(id, i + 1)))
        const allInitialIds = pageIdBatches.flatMap((ids) => (Array.isArray(ids) ? ids : []))
        const firstItems = await loadItemsBatched(allInitialIds, id, locale)
        this.states.set({ allItems: firstItems, loadedPages: initialPages })

        // Remaining pages load silently in the background
        if (initialPages < pages) this._readyPromise = this._loadAllInBackground(id, locale, initialPages, pages, this._generation)

        // Build type filter
        const typeFilter = this.shadowRoot.querySelector("#type-filter")
        typeFilter.allLabel = dict.all || "All"
        typeFilter.options = types.map((t) => ({ label: t, value: t }))
        typeFilter.value = this.states.get("activeType")
        typeFilter.addEventListener("filter", (e) => {
            this.states.set({ activeType: e.detail.value })
            this._handleFilterChange()
        })

        // Build rarity filter
        const rarityFilter = this.shadowRoot.querySelector("#rarity-filter")
        rarityFilter.allLabel = dict.all || "All"
        rarityFilter.maxVisible = rarities.length
        rarityFilter.options = rarities.map((r) => {
            const key = r.toLowerCase().replace(/\s+/g, "-")
            return { label: r, value: r, color: `var(--rarity-${key}, var(--color-accent))` }
        })
        rarityFilter.value = this.states.get("activeRarity")
        rarityFilter.addEventListener("filter", (e) => {
            const updates = { activeRarity: e.detail.value }
            if (e.detail.value) {
                const sort = this.states.get("sort")
                const rarityOpt = SORT_OPTIONS.find((o) => o.key === "rarity")
                if (sort === rarityOpt.asc || sort === rarityOpt.desc) updates.sort = "name"
            }
            this.states.set(updates)
            this._handleFilterChange()
        })

        // Wire ui-search component
        const searchEl = this.shadowRoot.querySelector("#search")
        searchEl.clear()
        this.states.set({ search: "" })

        searchEl.addEventListener("search", (e) => {
            const val = e.detail.value
            this.states.set({ search: val })
            // Feed current items as suggestion source
            searchEl.items = this.states.get("allItems").map((item) => {
                const key = (item.rarity || "").toLowerCase().replace(/\s+/g, "-")
                return { name: item.name, meta: item.rarity || "", color: `var(--rarity-${key}, var(--color-accent))` }
            })
            this._handleFilterChange()
        })

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
                this._displayPage = 1
                this.applyFilters()
            })
            return btn
        })
        const sortLabel = document.createElement("span")
        sortLabel.className = "sort-bar__label"
        sortLabel.textContent = "SORT BY "
        sortBar.replaceChildren(sortLabel, ...sortButtons)

        // Lock every sort button to the same width so switching active option
        // never shifts the search input. Measure at the widest possible indicator
        // text ("A→Z"), then restore empty dir slots and pin min-width.
        // Measure the widest button label, then add the arrow glyph + gap on top.
        // We can't rely on toggling .active + offsetWidth because the nested CSS selector
        // (.sort-bar button.active .sort-dir) may not reflow in time. Instead:
        //   1. Force all dirs to display:inline-block via inline style so they're measurable.
        //   2. Set arrow text and read the widest button.
        //   3. Restore dirs and pin min-width on all buttons.
        const buttons = [...sortBar.querySelectorAll("button")]
        const allDirs = buttons.map((b) => b.querySelector(".sort-dir"))
        allDirs.forEach((d) => {
            if (d) { d.style.display = "inline-block"; d.textContent = "↑" }
        })
        const maxBtnW = Math.max(...buttons.map((b) => b.offsetWidth))
        allDirs.forEach((d) => {
            if (d) { d.style.display = ""; d.textContent = "" }
        })
        buttons.forEach((b) => {
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
}

customElements.define("route-game", GAME)

export default GAME
