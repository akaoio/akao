import template from "./template.js"
import Router from "/core/Router.js"
import { Context } from "/core/Context.js"
import States from "/core/States.js"
import ITEM from "/UI/components/item/index.js"
import { html, render } from "/core/UI.js"
import logic from "./logic.js"

const INITIAL_PAGES = 3
const LOAD_MORE_PAGES = 1

const SORT_OPTIONS = [
    { key: "name", label: "Name", asc: "name", desc: "name-desc", indAsc: "↑", indDesc: "↓" },
    { key: "rarity", label: "Rarity", asc: "rarity-asc", desc: "rarity-desc", indAsc: "↑", indDesc: "↓" },
    { key: "price", label: "Price", asc: "value-asc", desc: "value-desc", indAsc: "↑", indDesc: "↓" }
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
        this.subs = []
        this._loadingAll = false
        this.render = this.render.bind(this)
        this.applyFilters = this.applyFilters.bind(this)
        this._applyFiltersWithLoad = this._applyFiltersWithLoad.bind(this)
        this.loadMore = this.loadMore.bind(this)
    }

    async connectedCallback() {
        this.subs.push(Context.on("locale", this.render))
        await this.render()

        // ── Sticky sentinel — detect when .catalog-sticky has stuck ──
        const sticky = this.shadowRoot.querySelector(".catalog-sticky")
        const sentinel = this.shadowRoot.querySelector(".sticky-sentinel")
        const pill = this.shadowRoot.querySelector("#catalog-pill")
        if (sticky && sentinel) {
            const stickyTopPx = parseInt(getComputedStyle(sticky).top) || 0

            // Sets the inline pixel width that acts as the "from" value for transitions.
            // Called after any change that may resize the pill (stuck, filter change).
            const syncPillWidth = () => {
                if (!pill) return
                if (window.matchMedia("(min-width: 1024px)").matches) return
                this._pillWidth = pill.getBoundingClientRect().width
                sticky.style.width = this._pillWidth + "px"
            }

            // Collapses to pill with an animated transition (used for explicit user actions).
            const animateCollapse = () => {
                // Set the numeric target width + remove .is-expanded in one style flush.
                // Browser sees: from 100% (CSS .is-stuck.is-expanded) → to _pillWidth px (inline).
                if (this._pillWidth) sticky.style.width = this._pillWidth + "px"
                sticky.classList.remove("is-expanded")
            }

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

            // Pill click → expand (animated).
            // Clear inline px width + add .is-expanded in one flush.
            // Browser sees: from _pillWidth px (inline) → to 100% (CSS .is-stuck.is-expanded).
            const expandSticky = () => {
                sticky.style.width = ""
                sticky.classList.add("is-expanded")
            }

            if (pill) {
                pill.addEventListener("click", expandSticky)
                this.subs.push(() => pill.removeEventListener("click", expandSticky))
            }

            // Individual pill segment actions — each stops propagation so the
            // general pill-expand doesn't also fire, then handles its own action.

            const pillSortEl = this.shadowRoot.querySelector("#pill-sort")
            if (pillSortEl) {
                const onPillSort = (e) => {
                    e.stopPropagation()
                    const sortCycle = SORT_OPTIONS.flatMap((o) => [o.asc, o.desc])
                    const current = this.states.get("sort")
                    const idx = sortCycle.indexOf(current)
                    const next = sortCycle[(idx + 1) % sortCycle.length]
                    this.states.set({ sort: next })
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
                    requestAnimationFrame(() => {
                        const selectWrap = this.shadowRoot.querySelector("#type-select")?.closest(".filter-select-wrap")
                        if (selectWrap && getComputedStyle(selectWrap).display !== "none") this.shadowRoot.querySelector("#type-select").focus()
                        else this.shadowRoot.querySelector("#type-tabs button")?.focus()
                    })
                }
                pillTypeEl.addEventListener("click", onPillType)
                this.subs.push(() => pillTypeEl.removeEventListener("click", onPillType))
            }

            const pillRarityDotEl = this.shadowRoot.querySelector("#pill-rarity-dot")
            if (pillRarityDotEl) {
                const onPillRarityDot = (e) => {
                    e.stopPropagation()
                    expandSticky()
                    requestAnimationFrame(() => {
                        const selectWrap = this.shadowRoot.querySelector("#rarity-select")?.closest(".filter-select-wrap")
                        if (selectWrap && getComputedStyle(selectWrap).display !== "none") this.shadowRoot.querySelector("#rarity-select").focus()
                        else this.shadowRoot.querySelector(".rarity-pills button")?.focus()
                    })
                }
                pillRarityDotEl.addEventListener("click", onPillRarityDot)
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

            // Collapse button → collapse (animated).
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

            // Store syncPillWidth so applyFilters can update the width when pill content changes.
            this._syncPillWidth = syncPillWidth
        }
    }

    disconnectedCallback() {
        this.subs.forEach((off) => off())
        ;["--game-primary", "--game-text-color", "--game-title-shadow"].forEach((v) => document.documentElement.style.removeProperty(v))
    }

    async _loadAllRemaining() {
        if (this._loadingAll) return
        this._loadingAll = true
        const grid = this.shadowRoot.querySelector("#items")
        if (grid) grid.classList.add("is-loading-all")

        const id = this.states.get("id")
        const locale = Context.get("locale")?.code || "en"
        const fromPage = this.states.get("loadedPages")
        const totalPages = this.states.get("totalPages")
        const count = totalPages - fromPage

        const pageIdBatches = await Promise.all(Array.from({ length: count }, (_, i) => logic.page(id, fromPage + i + 1)))
        const newItems = (await Promise.all(pageIdBatches.flatMap((ids) => (Array.isArray(ids) ? ids.map((itemId) => logic.item(id, itemId, locale)) : [])))).filter(Boolean)

        const allItems = [...this.states.get("allItems"), ...newItems]
        this.states.set({ allItems, loadedPages: totalPages })

        this._loadingAll = false
        if (grid) grid.classList.remove("is-loading-all")
    }

    async _applyFiltersWithLoad() {
        const anyFilter = this.states.get("activeType") || this.states.get("activeRarity") || this.states.get("search")
        if (anyFilter && this.states.get("loadedPages") < this.states.get("totalPages")) {
            this.applyFilters()
            await this._loadAllRemaining()
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
        const loadedPages = this.states.get("loadedPages")
        const totalPages = this.states.get("totalPages")
        const totalItems = this.states.get("totalItems")

        let filtered = allItems
        if (activeType) filtered = filtered.filter((i) => i.type === activeType)
        if (activeRarity) filtered = filtered.filter((i) => (i.rarity || "").toLowerCase() === activeRarity.toLowerCase())
        if (search) {
            const q = search.toLowerCase()
            filtered = filtered.filter((i) => (i.name || "").toLowerCase().includes(q))
        }
        filtered = logic.sort(filtered, sort, rarityOrder)

        // Count
        const countEl = this.shadowRoot.querySelector("#count-num")
        if (countEl) countEl.textContent = filtered.length

        // ── Sync collapsed pill ──
        const pillType = this.shadowRoot.querySelector("#pill-type")
        const pillRarityDot = this.shadowRoot.querySelector("#pill-rarity-dot")
        const pillCount = this.shadowRoot.querySelector("#pill-count-num")
        const pillSort = this.shadowRoot.querySelector("#pill-sort")
        const pillSearch = this.shadowRoot.querySelector("#pill-search")

        if (pillType)
            if (activeType) {
                pillType.textContent = activeType
                pillType.classList.add("active")
            } else {
                pillType.textContent = "All"
                pillType.classList.remove("active")
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

        if (pillCount) pillCount.textContent = filtered.length
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

        // Grid
        const grid = this.shadowRoot.querySelector("#items")
        const elements = filtered.map((item) => {
            const el = new ITEM()
            el.dataset.item = JSON.stringify(item)
            return el
        })
        grid.replaceChildren(...elements)

        // Load more button — hidden whenever a filter is active (all pages load eagerly then)
        const loadMoreBtn = this.shadowRoot.querySelector("#load-more")
        if (loadMoreBtn) {
            const anyFilterActive = !!(activeType || activeRarity || search)
            const hasMore = loadedPages < totalPages && !anyFilterActive
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

        const locale = Context.get("locale")?.code || "en"
        const pagesToLoad = Math.min(LOAD_MORE_PAGES, totalPages - loadedPages)
        const pageIdBatches = await Promise.all(Array.from({ length: pagesToLoad }, (_, i) => logic.page(id, loadedPages + i + 1)))
        const newItems = (await Promise.all(pageIdBatches.flatMap((ids) => (Array.isArray(ids) ? ids.map((itemId) => logic.item(id, itemId, locale)) : [])))).filter(Boolean)
        const allItems = [...this.states.get("allItems"), ...newItems]
        this.states.set({ allItems, loadedPages: loadedPages + pagesToLoad })

        if (loadMoreBtn) loadMoreBtn.disabled = false
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

        // Load first N pages in parallel
        const initialPages = Math.min(INITIAL_PAGES, pages)
        const pageIdBatches = await Promise.all(Array.from({ length: initialPages }, (_, i) => logic.page(id, i + 1)))
        const firstItems = (await Promise.all(pageIdBatches.flatMap((ids) => (Array.isArray(ids) ? ids.map((itemId) => logic.item(id, itemId, locale)) : [])))).filter(Boolean)
        this.states.set({ allItems: firstItems, loadedPages: initialPages })

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
            this._applyFiltersWithLoad()
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
            this._applyFiltersWithLoad()
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
                    this._applyFiltersWithLoad()
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
            this._applyFiltersWithLoad()
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
            this._applyFiltersWithLoad()
        })
        return btn
    }
}

customElements.define("route-game", GAME)

export default GAME
