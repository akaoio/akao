import template from "./template.js"
import { html, render } from "/core/UI.js"
import Events from "/core/Events.js"
import Component from "/core/UI/Component.js"

const _workCache = new Map()

function hashHue(str) {
    let hash = 0
    for (let i = 0; i < str.length; i++) {
        const char = str.charCodeAt(i)
        hash = (hash << 5) - hash + char
        hash = hash & hash
    }
    return Math.abs(hash) % 360
}

export async function cachedWork(data, salt) {
    const key = `${data}:${salt}`
    if (_workCache.has(key)) return _workCache.get(key)
    const result = await globalThis.sea.work(data, salt)
    _workCache.set(key, result)
    return result
}

export class IDENTICONS extends Component {
    static module = import.meta.url
    constructor() {
        super()
        this.template = template // Store for HMR
        this.events = new Events()
        this.$subs = []
        this.$id = 0
        this.$savedId = 0
        this.$total = 0
        this.$renderPending = false
        this._scrollToNew = false
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }

    get name() {
        return this.dataset.name || "item"
    }
    get size() {
        return Number(this.dataset.size) || 5
    }
    get step() {
        return Number(this.dataset.step) || 5
    }

    static get observedAttributes() {
        return ["data-seed"]
    }

    attributeChangedCallback(name, last, value) {
        if (name === "data-seed" && last !== value) {
            this.clear()
            this.render()
        }
    }

    get mode() {
        return this.dataset.mode || "carousel"
    }

    onconnect() {
        this.$container = this.shadowRoot.querySelector("#container")
        this.$loader    = this.shadowRoot.querySelector("#loader")

        const onWheel = (e) => {
            if (e.deltaY === 0) return
            e.preventDefault()
            const delta = (e.deltaMode === 1 ? e.deltaY * 40 : e.deltaY) * 3
            this.$container.scrollBy({ left: delta, behavior: "auto" })
        }
        this.listen(this.$container, "wheel", onWheel, { passive: false })

        // carousel mode: ‹ / › page through slots
        const $dec = this.shadowRoot.querySelector("#status-decrease")
        const $inc = this.shadowRoot.querySelector("#status-increase")
        this.listen($dec, "click", () => this.events.emit("decrease"))
        this.listen($inc, "click", () => this.events.emit("increase", { scrollToNew: true }))

        // picker mode: + adds a new slot (cap enforced by the consumer)
        const $add = this.shadowRoot.querySelector("#status-add")
        this.listen($add, "click", () => this.events.emit("add"))

        this._applyMode()
    }

    scrollTo(id, behavior = "instant") {
        const item = this.$container?.querySelector(`input#i${id}`)?.closest(".item")
        if (!item) return
        if (behavior !== "smooth") {
            item.scrollIntoView({ behavior, inline: "center", block: "nearest" })
            return
        }
        // Manual smooth scroll so we can control duration (~345ms ≈ browser default × 1.15)
        const container = this.$container
        const containerRect = container.getBoundingClientRect()
        const itemRect = item.getBoundingClientRect()
        const target = container.scrollLeft + itemRect.left - containerRect.left
                       - (containerRect.width - itemRect.width) / 2
        const start = container.scrollLeft
        const delta = target - start
        const duration = 345
        let startTime = null
        const easeInOut = (t) => t < 0.5 ? 2 * t * t : -1 + (4 - 2 * t) * t
        const step = (now) => {
            if (!startTime) startTime = now
            const elapsed = Math.min((now - startTime) / duration, 1)
            container.scrollLeft = start + delta * easeInOut(elapsed)
            if (elapsed < 1) requestAnimationFrame(step)
        }
        requestAnimationFrame(step)
    }

    set id(value) {
        this.$id = Number(value)
        this._updateStatus()
    }

    set savedId(value) {
        this.$savedId = Number(value)
        this._updateStatus()
        // Stamp data-saved on the committed item, clear all others
        this.$container?.querySelectorAll(".item[data-saved]").forEach((el) => delete el.dataset.saved)
        const item = this.$container?.querySelector(`input#i${this.$savedId}`)?.closest(".item")
        if (item) item.dataset.saved = ""
    }

    set previewId(value) {
        // Clear previous previewed item
        this.$container?.querySelectorAll(".item[data-previewed]").forEach((el) => delete el.dataset.previewed)
        if (value === null || value === undefined) return
        const item = this.$container?.querySelector(`input#i${Number(value)}`)?.closest(".item")
        if (item) item.dataset.previewed = ""
    }

    set total(value) {
        this.$total = Number(value)
        this._updateStatus()
        this.render()
    }

    set addDisabled(value) {
        const $add = this.shadowRoot?.querySelector("#status-add")
        if ($add) $add.disabled = !!value
    }

    /** Burst the outline of the committed item then restore it. */
    flashSelected() {
        const input = this.$container?.querySelector(`input#i${this.$savedId}`)
        const label = input?.nextElementSibling
        if (!label) return
        label.classList.remove("switching")
        // Force reflow so re-adding the class restarts the animation
        void label.offsetWidth
        label.classList.add("switching")
        label.addEventListener("animationend", () => label.classList.remove("switching"), { once: true })
    }

    async create() {
        if (this.$container.children.length >= this.$total) return
        const templates = []
        for (let id = this.$container.children.length; id < this.$total; id++) {
            const seed = await cachedWork(this.dataset.seed, id)
            const hue = hashHue(seed)
            const select = () => {
                this.events.emit("select", { id })
            }
            templates.push(html`
                <span class="item" style="--item-hue: ${hue}">
                    <input id="i${id}" type="radio" name="${this.name}" value="${id}" />
                    <label
                        for="i${id}"
                        ${({ element }) => {
                            element.addEventListener("click", select)
                            this.$subs.push(() => element.removeEventListener("click", select))
                        }}>
                        <ui-identicon data-size="${this.size}" data-seed="${seed}" />
                    </label>
                </span>
            `)
        }
        render(templates, this.$container, { append: true })
        // Restore data-saved on the committed item after new nodes are injected
        const savedItem = this.$container.querySelector(`input#i${this.$savedId}`)?.closest(".item")
        if (savedItem) savedItem.dataset.saved = ""
    }

    remove() {
        const count = this.$container.children.length
        const min = Math.max(this.step, this.$id + 1)
        if (count <= min) return
        for (let i = 0; i < Math.min(this.step, count - this.$total); i++) this.$container.removeChild(this.$container.lastChild)
        if (this.$container.children.length > this.$total) this.remove()
    }

    clear() {
        this.$subs.forEach((off) => off())
        this.$subs = []
        if (!this.$container) return
        while (this.$container.firstChild) this.$container.removeChild(this.$container.firstChild)
    }

    render(scrollToNew = false) {
        if (scrollToNew) this._scrollToNew = true
        if (this.$renderPending) return
        this.$renderPending = true
        queueMicrotask(async () => {
            this.$renderPending = false
            const doScroll = this._scrollToNew
            this._scrollToNew = false
            if (!this.$total || !this.dataset.seed || !this.$container) return
            if (this.$container.children.length < this.$total) {
                this._setLoading(true)
                await this.create()
                if (doScroll) {
                    this.scrollTo(this.$id, "smooth")
                }
                this._setLoading(false)
            }
            if (this.$container.children.length > this.$total) this.remove()
        })
    }

    _setLoading(on) {
        if (!this.$loader) return
        this.$loader.hidden = !on
        // Disable both nav buttons while generating — add button is unaffected
        const $dec = this.shadowRoot.querySelector("#status-decrease")
        const $inc = this.shadowRoot.querySelector("#status-increase")
        if ($dec) $dec.disabled = on
        if ($inc) $inc.disabled = on
    }

    _updateStatus() {
        const root = this.shadowRoot
        if (!root) return
        const saved    = this.$savedId
        const selected = this.$id
        const total    = this.$total
        const hasDiff  = selected !== saved

        root.querySelector("#status-saved").textContent       = `#${saved + 1}`
        root.querySelector("#status-preview-num").textContent = `#${selected + 1}`
        root.querySelector("#status-preview").hidden = !hasDiff
        root.querySelector("#status-arrow").hidden   = !hasDiff
        root.querySelector("#status-total").textContent = total ? `${total} ${this.name}s` : ""
        root.querySelector("#status-decrease").disabled = total - this.step <= selected
    }

    _applyMode() {
        this.setAttribute("data-mode", this.mode)
    }
}

customElements.define("ui-identicons", IDENTICONS)
export default IDENTICONS
