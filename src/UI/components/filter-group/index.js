import template from "./template.js"
import { render } from "/core/UI.js"
import Component from "/core/UI/Component.js"

/**
 * <ui-filter-group>
 *
 * A label + tab-buttons (desktop) / native select (mobile ≤767px) filter row.
 * Emits a "filter" CustomEvent when the active value changes.
 *
 * Attributes:
 *   data-pill   — applies pill/rounded shape to tab buttons
 *   bordered    — adds a top border separator (for stacked groups)
 *
 * Properties:
 *   options     — Array of { label, value, color? } objects.
 *                 value=null/"" is treated as "All" (clears the filter).
 *                 color sets --filter-item-color on the button (for rarity dots).
 *   value       — currently active value (null = "All")
 *   allLabel    — label for the "all" option (default: "All")
 *   maxVisible  — max tab buttons before overflow toggle appears (default: 6)
 *
 * Events:
 *   filter      — CustomEvent({ detail: { value } }) — value is null when "All"
 */
export class FILTER_GROUP extends Component {
    static module = import.meta.url

    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this._options = []
        this._value = null
        this._allLabel = "All"
        this._maxVisible = 6
    }

    get options() { return this._options }
    set options(val) {
        this._options = Array.isArray(val) ? val : []
        this._build()
    }

    get value() { return this._value }
    set value(val) {
        this._value = val ?? null
        this._syncActive()
    }

    get allLabel() { return this._allLabel }
    set allLabel(val) {
        this._allLabel = val || "All"
        this._build()
    }

    get maxVisible() { return this._maxVisible }
    set maxVisible(val) {
        this._maxVisible = Number(val) || 6
        this._build()
    }

    onconnect() {
        this.$tabs = this.shadowRoot.querySelector("#tabs")
        this.$select = this.shadowRoot.querySelector("#select")
        this.$choices = this.shadowRoot.querySelector("#choices")
        this._build()
    }

    // ── Build ───────────────────────────────────────────────────────────────

    _build() {
        if (!this.$tabs) return

        const all = [{ label: this._allLabel, value: null }, ...this._options]

        // ── Tab buttons ──
        this.$tabs.classList.remove("expanded")
        const maxV = this._maxVisible

        const buttons = all.map((opt, idx) => {
            const btn = document.createElement("button")
            btn.textContent = opt.label
            btn.dataset.value = opt.value ?? ""
            if (opt.color) {
                btn.dataset.colorKey = "true"
                btn.style.setProperty("--filter-item-color", opt.color)
            }
            if (idx > maxV) btn.dataset.overflow = "true"
            btn.addEventListener("click", () => this._select(opt.value ?? null))
            return btn
        })

        const overflowCount = Math.max(0, this._options.length - maxV)
        if (overflowCount > 0) {
            const toggle = document.createElement("button")
            toggle.className = "filter-tabs__toggle"
            toggle.textContent = `+${overflowCount} more`
            toggle.addEventListener("click", () => {
                const expanded = this.$tabs.classList.toggle("expanded")
                toggle.textContent = expanded ? "Show less" : `+${overflowCount} more`
            })
            buttons.push(toggle)
        }

        this.$tabs.replaceChildren(...buttons)

        // ── Select (mobile) ──
        const opts = all.map(({ label, value }) => {
            const opt = document.createElement("option")
            opt.value = value ?? ""
            opt.textContent = label
            return opt
        })
        this.$select.replaceChildren(...opts)
        this.$select.onchange = () => this._select(this.$select.value || null)

        this._syncActive()
    }

    _select(value) {
        this._value = value
        this._syncActive()
        this.dispatchEvent(new CustomEvent("filter", { detail: { value }, bubbles: true, composed: true }))
    }

    _syncActive() {
        if (!this.$tabs) return
        const current = this._value ?? ""

        this.$tabs.querySelectorAll("button[data-value]").forEach((btn) => {
            btn.classList.toggle("active", btn.dataset.value === (current === null ? "" : current))
        })

        this.$select.value = current ?? ""
        this.$select.classList.toggle("active", !!current)

        // Propagate accent color to both tabs and select
        const active = this._options.find((o) => o.value === current)
        if (active?.color) this.$choices.style.setProperty("--filter-accent", active.color)
        else this.$choices.style.removeProperty("--filter-accent")
    }
}

customElements.define("ui-filter-group", FILTER_GROUP)

export default FILTER_GROUP
