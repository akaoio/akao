import "/UI/components/svg/index.js"
import styles from "./styles.css.js"
import { html, render } from "/core/UI.js"

/**
 * <ui-option-list>
 *
 * Renders a selectable list of options — icons, names, and an optional
 * trailing badge for supplementary info (e.g. chain standard, coin full name).
 *
 * Methods:
 *   setOptions(options)   — [{ value, _name, _symbol, _meta? }]
 *   setSelection(value)   — highlight the active row
 *
 * Events (bubble):
 *   change  — detail: { value }
 */
export class OPTION_LIST extends HTMLElement {
    static module = import.meta.url

    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(html`${styles}<ul id="list"></ul>`, this.shadowRoot)
        this._options = []
    }

    connectedCallback() {
        this.$list = this.shadowRoot.querySelector("#list")
    }

    setOptions(options) {
        this._options = options
        this.$list.innerHTML = ""
        for (const opt of options) {
            const li = document.createElement("li")
            li.dataset.value = opt.value

            const icon = document.createElement("ui-svg")
            if (opt._symbol) icon.dataset.src = `/images/cryptos/${opt._symbol}`

            const name = document.createElement("span")
            name.textContent = opt._name || opt.value

            li.append(icon, name)

            if (opt._meta) {
                const badge = document.createElement("span")
                badge.className = "meta"
                badge.textContent = opt._meta
                li.append(badge)
            }

            li.addEventListener("click", () => {
                this.dispatchEvent(new CustomEvent("change", { detail: { value: opt.value }, bubbles: true }))
            })

            this.$list.append(li)
        }
    }

    setSelection(value) {
        this.$list?.querySelectorAll("li").forEach((li) => {
            if (li.dataset.value === value) li.dataset.selected = "true"
            else delete li.dataset.selected
        })
    }
}

customElements.define("ui-option-list", OPTION_LIST)
export default OPTION_LIST
