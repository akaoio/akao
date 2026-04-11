import template from "./template.js"
import { Context } from "/core/Context.js"
import { events } from "/core/Events.js"
import { Indexes, Lives, Chains } from "/core/Stores.js"
import Route from "/core/UI/Route.js"
import logic from "./logic.js"

const PAGE_SIZE = 25

export class POOLS extends Route {
    static module = import.meta.url
    constructor() {
        super(template)

        this._elements = new Map()
        this._allRows = []
        this._page = 1
        this._hasData = false

        this._onLivePools = this._onLivePools.bind(this)
        this._onFiat = this._onFiat.bind(this)
        this._onParams = this._onParams.bind(this)
        this.loadMore = this.loadMore.bind(this)
    }

    async onconnect() {
        const sr = this.shadowRoot
        this.$list = sr.querySelector("#list")
        this.$empty = sr.querySelector("#empty")
        this.$skeleton = sr.querySelector("#skeleton")
        this.$loadMore = sr.querySelector("#load-more")
        this.$loadLabel = sr.querySelector("#load-more-label")
        this.$poolCount = sr.querySelector("#pool-count")
        this.$poolCountLabel = sr.querySelector("#pool-count-label")
        this.$loadingStatus = sr.querySelector("#loading-status")
        this.$liveDot = sr.querySelector("#live-dot")
        this.$loader = sr.querySelector("#loader")
        this.$content = sr.querySelector(".pools-content")
        this.$chainSelect = sr.querySelector("#chain-select")
        this.$dexPills = sr.querySelector("#dex-pills")

        this.listen(this.$loadMore, "click", this.loadMore)
        this._initStickyObserver()

        await this.loadcache()

        // Subscribe before the final check — prevents missing an event that
        // fires between loadcache() completing and the listener being attached.
        this.sub(
            events.on("Lives.pools", this._onLivePools),
            Context.on("fiat", this._onFiat),
            Context.on("params", this._onParams)
        )

        if (this._hasData) this._showData()
    }

    ondisconnect() {
        if (this._stickyObserver) this._stickyObserver.disconnect()
    }

    async loadcache() {
        const cached = await Indexes.Lives.get("pools").once()
        const flat = logic.flatten(cached)
        if (Object.keys(flat).length) {
            if (!Lives.pools) Lives.pools = {}
            for (const [chain, chainPools] of Object.entries(flat)) {
                if (!Lives.pools[chain]) Lives.pools[chain] = {}
                Object.assign(Lives.pools[chain], chainPools)
            }
            this._hasData = true
        }
        // Sync worker may have already populated Lives.pools before this route mounted
        if (!this._hasData && Lives.pools && Object.keys(Lives.pools).length)
            this._hasData = true
    }

    _onLivePools() {
        this._hasData = true
        this._showData()
    }

    _onFiat() {
        this._showData()
    }

    _onParams() {
        this._page = 1
        this._showData()
    }

    _showData() {
        const params = Context.get("params")
        const rows = logic.filter(Lives.pools, params)
        this._allRows = rows

        // One-time transition: loader → content, loading-status → pool count
        if (this.$loader && !this.$loader.hidden) {
            this.$loader.hidden = true
            this.$content.removeAttribute("hidden")
            if (this.$loadingStatus) this.$loadingStatus.hidden = true
            if (this.$poolCount) this.$poolCount.removeAttribute("hidden")
            if (this.$poolCountLabel) this.$poolCountLabel.removeAttribute("hidden")
            if (this.$liveDot) this.$liveDot.removeAttribute("hidden")
        }

        if (this.$poolCount) {
            const next = String(rows.length)
            if (this.$poolCount.textContent !== next) {
                this.$poolCount.textContent = next
                this.$poolCount.classList.remove("ticking")
                void this.$poolCount.offsetWidth // force reflow to restart animation
                this.$poolCount.classList.add("ticking")
            }
        }

        this._hideSkeleton()

        if (!rows.length) {
            this.$list.style.display = "none"
            this.$empty.removeAttribute("hidden")
            this.$empty.querySelector(".empty-state__text").textContent =
                Context.get(["dictionary", "nopoolFound"]) || "No pools found"
            this._updateLoadMore()
            this._buildFilters()
            return
        }

        this.$empty.setAttribute("hidden", "")
        this.$list.style.display = ""
        this._diffRender()
        this._updateLoadMore()
        this._buildFilters()
    }

    _hideSkeleton() {
        if (this.$skeleton) this.$skeleton.setAttribute("hidden", "")
    }

    _diffRender() {
        const visible = this._allRows.slice(0, this._page * PAGE_SIZE)
        const newKeys = new Set(visible.map((r) => `${r.chain}-${r.address}`))

        for (const [key, el] of this._elements)
            if (!newKeys.has(key)) {
                el.remove()
                this._elements.delete(key)
            }

        for (const { chain, address } of visible) {
            const key = `${chain}-${address}`
            if (this._elements.has(key)) continue

            const el = document.createElement("ui-pool")
            el.dataset.chain = chain
            el.dataset.address = address
            el.classList.add("entering")
            el.addEventListener("animationend", () => el.classList.remove("entering"), { once: true })
            this.$list.appendChild(el)
            this._elements.set(key, el)
        }
    }

    loadMore() {
        this._page++
        this._diffRender()
        this._updateLoadMore()
    }

    _updateLoadMore() {
        const total = this._allRows.length
        const shown = this._page * PAGE_SIZE
        const remaining = Math.max(0, total - shown)
        const hasMore = remaining > 0

        this.$loadMore.hidden = !hasMore
        if (this.$loadLabel)
            this.$loadLabel.textContent = hasMore
                ? `Load ${Math.min(remaining, PAGE_SIZE)} more — ${remaining} remaining`
                : "Load more"
    }

    _buildFilters() {
        if (!this.$chainSelect || !this.$dexPills) return
        const { chains, dexes } = logic.extractFilters(Lives.pools)
        const params = Context.get("params") || {}
        const activeChain = params.chain ? Number(params.chain) : null
        const activeDex = params.dex ? params.dex.toLowerCase() : null

        // Chain — native select
        const chainOptions = [
            this._makeOption("All chains", ""),
            ...chains.map((id) => {
                const name = Chains[id]?.configs?.name || `Chain ${id}`
                return this._makeOption(name, id)
            }),
        ]
        this.$chainSelect.replaceChildren(...chainOptions)
        this.$chainSelect.value = activeChain !== null ? String(activeChain) : ""
        this.$chainSelect.classList.toggle("active", activeChain !== null)
        if (!this.$chainSelect._wired) {
            this.$chainSelect.addEventListener("change", (e) => {
                this._applyFilter("chain", e.target.value || null)
                e.target.classList.toggle("active", !!e.target.value)
            })
            this.$chainSelect._wired = true
        }

        // DEX — pills
        const dexPills = [
            this._makePill("All", null, "dex", activeDex === null),
            ...dexes.map((id) => this._makePill(id.toUpperCase(), id, "dex", activeDex === id)),
        ]
        this.$dexPills.replaceChildren(...dexPills)
    }

    _makeOption(label, value) {
        const opt = document.createElement("option")
        opt.value = value
        opt.textContent = label
        return opt
    }

    _makePill(label, value, filterKey, isActive) {
        const btn = document.createElement("button")
        btn.className = "filter-pill" + (isActive ? " active" : "")
        btn.textContent = label
        btn.addEventListener("click", () => this._applyFilter(filterKey, value))
        return btn
    }

    _applyFilter(key, value) {
        const params = { ...(Context.get("params") || {}) }

        if (value === null) delete params[key]
        else params[key] = String(value)

        Context.set({ params })

        try {
            const url = new URL(globalThis.location.href)
            for (const k of ["chain", "dex"])
                if (params[k]) url.searchParams.set(k, params[k])
                else url.searchParams.delete(k)
            globalThis.history.replaceState({ ...globalThis.history.state, params }, "", url)
        } catch (_) {
            // non-browser env
        }
    }

    _initStickyObserver() {
        const strip = this.shadowRoot.querySelector(".filter-strip")
        const sentinel = this.shadowRoot.querySelector(".sticky-sentinel")
        if (!strip || !sentinel) return

        const topPx = parseInt(getComputedStyle(strip).top) || 0
        this._stickyObserver = new IntersectionObserver(
            ([entry]) => strip.classList.toggle("is-stuck", !entry.isIntersecting),
            { rootMargin: `-${topPx}px 0px 0px 0px`, threshold: 0 }
        )
        this._stickyObserver.observe(sentinel)
    }
}

customElements.define("route-pools", POOLS)

export default POOLS
