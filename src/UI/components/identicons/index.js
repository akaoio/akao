import template from "./template.js"
import { html, render } from "/core/UI.js"
import Events from "/core/Events.js"
import Component from "/core/UI/Component.js"
import zen from "/core/ZEN.js"

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
    const result = await zen.work(data, salt)
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

    onconnect() {
        this.$container = this.shadowRoot.querySelector("#container")
        this.$loader = this.shadowRoot.querySelector("#loader")

        const onWheel = (e) => {
            if (e.deltaY === 0) return
            e.preventDefault()
            const delta = (e.deltaMode === 1 ? e.deltaY * 40 : e.deltaY) * 3
            this.$container.scrollBy({ left: delta, behavior: "auto" })
        }
        this.listen(this.$container, "wheel", onWheel, { passive: false })

        const onDecrease = () => this.events.emit("decrease")
        const onIncrease = () => this.events.emit("increase", { scrollToNew: true })
        const $dec = this.shadowRoot.querySelector("#status-decrease")
        const $inc = this.shadowRoot.querySelector("#status-increase")
        this.listen($dec, "click", onDecrease)
        this.listen($inc, "click", onIncrease)
    }

    scrollTo(id, behavior = "instant") {
        const item = this.$container?.querySelector(`input#i${id}`)?.closest(".item")
        if (item) item.scrollIntoView({ behavior, inline: "center", block: "nearest" })
    }

    set id(value) {
        this.$id = Number(value)
        const input = this.$container?.querySelector(`input#i${this.$id}`)
        if (input) input.checked = true
        this._updateStatus()
    }

    set savedId(value) {
        this.$savedId = Number(value)
        this._updateStatus()
    }

    set total(value) {
        this.$total = Number(value)
        this._updateStatus()
        this.render()
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
        const input = this.$container.querySelector(`input#i${this.$id}`)
        if (input) input.checked = true
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
                const firstNewIndex = this.$container.children.length
                this._setLoading(true)
                await this.create()
                if (doScroll) {
                    const firstNew = this.$container.children[firstNewIndex]
                    if (firstNew) firstNew.scrollIntoView({ behavior: "smooth", inline: "start", block: "nearest" })
                }
                this._setLoading(false)
            }
            if (this.$container.children.length > this.$total) this.remove()
        })
    }

    _setLoading(on) {
        if (!this.$loader) return
        this.$loader.hidden = !on
        const $inc = this.shadowRoot.querySelector("#status-increase")
        if ($inc) $inc.disabled = on
    }

    _updateStatus() {
        const root = this.shadowRoot
        if (!root) return
        const saved = this.$savedId
        const selected = this.$id
        const total = this.$total
        const hasDiff = selected !== saved

        root.querySelector("#status-saved").textContent = `#${saved + 1}`
        root.querySelector("#status-preview-num").textContent = `#${selected + 1}`
        root.querySelector("#status-preview").hidden = !hasDiff
        root.querySelector("#status-arrow").hidden = !hasDiff
        root.querySelector("#status-total").textContent = total ? `${total} avatars` : ""
        root.querySelector("#status-decrease").disabled = total - this.step <= selected
    }
}

customElements.define("ui-identicons", IDENTICONS)
export default IDENTICONS
