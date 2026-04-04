import { html, render } from "/core/UI.js"
import States from "/core/States.js"
import { Context } from "/core/Context.js"
import template from "./template.js"

export class TOKENSELECT extends HTMLElement {
    constructor() {
        super()
        this.states = new States({ selected: null, filter: "", open: false, options: [] })
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
        this.toggle = this.toggle.bind(this)
        this.search = this.search.bind(this)
        this.close = this.close.bind(this)
    }

    connectedCallback() {
        this.$trigger = this.shadowRoot.querySelector("button.trigger")
        this.$dropdown = this.shadowRoot.querySelector(".dropdown")
        this.$search = this.shadowRoot.querySelector(".search")
        this.$options = this.shadowRoot.querySelector(".options")
        this.$label = this.shadowRoot.querySelector(".label")
        this.$icon = this.shadowRoot.querySelector("button.trigger ui-svg")

        this.$trigger.addEventListener("click", this.toggle)
        this.$search.addEventListener("input", this.search)
        document.addEventListener("click", this.close)

        this.$search.placeholder = Context.get(["dictionary", "searchToken"]) || "Search token"

        this.subscriptions.push(
            this.states.on("options", () => this.renderlist()),
            this.states.on("filter", () => this.renderlist()),
            this.states.on("selected", () => this.renderselected()),
            this.states.on("open", ({ value }) => {
                if (value) this.$dropdown.removeAttribute("hidden")
                else this.$dropdown.setAttribute("hidden", "")
            }),
            () => {
                this.$trigger.removeEventListener("click", this.toggle)
                this.$search.removeEventListener("input", this.search)
                document.removeEventListener("click", this.close)
            }
        )
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }

    setoptions(options) {
        this.states.set({ options: options || [] })
    }

    toggle(e) {
        e.stopPropagation()
        this.states.set({ open: !this.states.get("open") })
        if (this.states.get("open")) {
            this.$search.value = ""
            this.states.set({ filter: "" })
            this.$search.focus()
        }
    }

    search(e) {
        this.states.set({ filter: e.target.value.toLowerCase() })
    }

    close(e) {
        if (!this.contains(e.target)) this.states.set({ open: false })
    }

    select(option) {
        this.states.set({ selected: option, open: false })
        this.dispatchEvent(new CustomEvent("select", { detail: option, bubbles: true, composed: true }))
    }

    renderselected() {
        const selected = this.states.get("selected")
        this.$label.textContent = selected?.configs?.name || Context.get(["dictionary", "selectToken"]) || "Select token"
        if (!selected) { this.$icon.dataset.src = ""; return }
        this.$icon.dataset.src = selected.configs?.symbol ? `/images/cryptos/${selected.configs.symbol}` : ""
    }

    renderlist() {
        const filter = this.states.get("filter")
        const options = this.states.get("options") || []
        const filtered = filter
            ? options.filter((o) => (o.configs?.name || "").toLowerCase().includes(filter))
            : options

        const items = filtered.map((option) => {
            const el = document.createElement("div")
            el.className = "option"
            render(html`
                <ui-svg data-src="/images/cryptos/${option.configs?.symbol || ""}"></ui-svg>
                <span>${option.configs?.name || option.address}</span>
            `, el)
            el.addEventListener("click", () => this.select(option))
            return el
        })

        this.$options.innerHTML = ""
        items.forEach((el) => this.$options.appendChild(el))
    }
}

customElements.define("ui-token-select", TOKENSELECT)

export default TOKENSELECT
