import template from "./template.js"
import { render } from "/core/UI.js"
import Component from "/core/UI/Component.js"

/**
 * <ui-search>
 *
 * Attributes:
 *   placeholder  — input placeholder text
 *   value        — current search string (reflected)
 *   min-chars    — minimum chars before suggestions open (default: 2)
 *
 * Properties:
 *   items        — Array of { name, meta?, color? } objects used to build suggestions.
 *                  Set via JS: el.items = [...].  Each item with a `color` receives
 *                  --suggestion-meta-color so rarity-keyed colors work out of the box.
 *
 * Events:
 *   search       — CustomEvent({ detail: { value } }) fired on every input change
 *   select       — CustomEvent({ detail: { item } }) fired when a suggestion is chosen
 */
export class SEARCH extends Component {
    static module = import.meta.url

    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this._items = []
        this._highlighted = -1
    }

    static get observedAttributes() {
        return ["placeholder", "value", "min-chars"]
    }

    attributeChangedCallback(name, _, next) {
        if (name === "placeholder") {
            const input = this.shadowRoot?.querySelector(".search-input")
            if (input) input.placeholder = next || ""
        }
        if (name === "value") {
            const input = this.shadowRoot?.querySelector(".search-input")
            if (input && input.value !== next) input.value = next || ""
        }
    }

    get minChars() {
        return Number(this.getAttribute("min-chars") ?? 2)
    }

    get items() { return this._items }
    set items(val) {
        this._items = Array.isArray(val) ? val : []
    }

    onconnect() {
        this.$input = this.shadowRoot.querySelector(".search-input")
        this.$list = this.shadowRoot.querySelector(".search-suggestions")

        // Reflect placeholder attribute set before connect
        const ph = this.getAttribute("placeholder")
        if (ph) this.$input.placeholder = ph

        const val = this.getAttribute("value")
        if (val) this.$input.value = val

        this.$input.addEventListener("input", this._onInput.bind(this))
        this.$input.addEventListener("focus", this._onFocus.bind(this))
        this.$input.addEventListener("blur", this._onBlur.bind(this))
        this.$input.addEventListener("keydown", this._onKeydown.bind(this))
    }

    // ── Public API ──────────────────────────────────────────────────────────

    clear() {
        this.$input.value = ""
        this._close()
    }

    focus() {
        this.$input?.focus()
    }

    // ── Internal ────────────────────────────────────────────────────────────

    _onInput(e) {
        const val = e.target.value.trim()
        this._highlighted = -1
        this._open(val)
        this.dispatchEvent(new CustomEvent("search", { detail: { value: val }, bubbles: true, composed: true }))
    }

    _onFocus(e) {
        this._open(e.target.value.trim())
    }

    _onBlur() {
        // Delay so a mousedown on a suggestion can fire first
        setTimeout(() => this._close(), 150)
    }

    _onKeydown(e) {
        const lis = [...this.$list.querySelectorAll(".search-suggestion")]
        const hi = this._highlighted

        if (e.key === "ArrowDown") {
            e.preventDefault()
            lis[hi]?.classList.remove("highlighted")
            this._highlighted = Math.min(hi + 1, lis.length - 1)
            lis[this._highlighted]?.classList.add("highlighted")
        } else if (e.key === "ArrowUp") {
            e.preventDefault()
            lis[hi]?.classList.remove("highlighted")
            this._highlighted = Math.max(hi - 1, 0)
            lis[this._highlighted]?.classList.add("highlighted")
        } else if (e.key === "Enter" && hi >= 0) {
            e.preventDefault()
            lis[hi]?.dispatchEvent(new MouseEvent("mousedown"))
        } else if (e.key === "Escape") {
            this._close()
            this.$input.blur()
        }
    }

    _open(query) {
        if (!query || query.length < this.minChars) return this._close()

        const q = query.toLowerCase()
        const matches = this._items
            .filter((item) => (item.name || "").toLowerCase().includes(q))
            .slice(0, 8)

        if (!matches.length) return this._close()

        const lis = matches.map((item, idx) => {
            const li = document.createElement("li")
            li.className = "search-suggestion"
            li.setAttribute("role", "option")
            if (idx === this._highlighted) li.classList.add("highlighted")

            const nameEl = document.createElement("span")
            nameEl.className = "suggestion__name"
            nameEl.textContent = item.name || ""

            li.append(nameEl)

            if (item.meta != null) {
                const metaEl = document.createElement("span")
                metaEl.className = "suggestion__meta"
                metaEl.textContent = item.meta
                if (item.color) metaEl.style.setProperty("--suggestion-meta-color", item.color)
                li.append(metaEl)
            }

            li.addEventListener("mousedown", (e) => {
                e.preventDefault()
                this.$input.value = item.name
                this._close()
                this.dispatchEvent(new CustomEvent("search", { detail: { value: item.name }, bubbles: true, composed: true }))
                this.dispatchEvent(new CustomEvent("select", { detail: { item }, bubbles: true, composed: true }))
            })
            return li
        })

        this.$list.replaceChildren(...lis)
        this.$list.classList.add("open")
    }

    _close() {
        this.$list.classList.remove("open")
        this._highlighted = -1
    }
}

customElements.define("ui-search", SEARCH)

export default SEARCH
