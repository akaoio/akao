import template from "./template.js"
import States from "/core/States.js"
import { html, render } from "/core/UI.js"
import Component from "/core/UI/Component.js"

export class PICKER extends Component {
    static module = import.meta.url
    constructor() {
        super()
        this.template = template
        this.states = new States({ options: [], selected: null })
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.show = this.show.bind(this)
        this.close = this.close.bind(this)
        this.select = this.select.bind(this)
        this.render = this.render.bind(this)
        this._onSearch = this._onSearch.bind(this)
    }

    static get observedAttributes() {
        return ["data-name", "data-selected", "data-mode", "data-searchable", "data-placeholder"]
    }

    attributeChangedCallback(name, last, value) {
        if (last === value) return
        if (name === "data-name" || name === "data-selected") {
            this.states.set({ [name.replace("data-", "")]: value })
        }
        if (name === "data-placeholder") {
            const input = this.shadowRoot?.querySelector(".picker-search")
            if (input) input.placeholder = value
        }
    }

    get mode() { return this.dataset.mode || "default" }
    get searchable() { return this.hasAttribute("data-searchable") }

    onconnect() {
        this.modal = this.shadowRoot.querySelector("ui-modal")
        this.modal.dataset.header = this.dataset.header

        this.$search = this.shadowRoot.querySelector(".picker-search")
        this.$searchSlot = this.shadowRoot.querySelector(".search-slot")

        // Show/hide search input based on data-searchable
        this.$searchSlot.hidden = !this.searchable
        if (this.searchable) {
            this.$search.placeholder = this.dataset.placeholder || ""
            this.$search.addEventListener("input", this._onSearch)
        }

        this.watch(this.states, "options", this.render, true)
        this.listen(this.modal, "change", (e) => {
            if (e.target.type === "radio" && e.target.name === this.name) {
                this.select(e.target.value)
                this.modal.close()
            }
        })
    }

    show() {
        this.modal.showModal()
        if (this.searchable) {
            this.$search.value = ""
            this._filter("")
            // Focus after dialog opening animation
            setTimeout(() => this.$search.focus(), 50)
        }
    }

    close() {
        this.modal.close()
    }

    get name() {
        return this.states.get("name") || this.dataset.name
    }

    get selected() {
        return this.states.get("selected") || this.dataset.selected
    }

    select(value) {
        this.states.set({ selected: value })
        this.dataset.selected = value
        if (typeof this.callback === "function") this.callback(value)
    }

    _onSearch(e) {
        this._filter(e.target.value.trim().toLowerCase())
    }

    _filter(query) {
        const rows = this.modal.querySelectorAll("label")
        rows.forEach((label) => {
            const text = label.dataset.search || label.textContent
            label.closest(".picker-row").hidden = query ? !text.toLowerCase().includes(query) : false
        })
    }

    render() {
        const name = this.name
        const isToken = this.mode === "token"

        const options = [...this.states.get("options")].sort((a, b) => (a.symbol || a.value).localeCompare(b.symbol || b.value)).map((option) => {
            if (isToken) {
                return html`
                    <div class="picker-row">
                        <input id="pick-${option.value}" type="radio" name="${name}" value="${option.value}" ${option.value === this.selected ? "checked" : ""} />
                        <label for="pick-${option.value}" data-search="${option.symbol || ""}">
                            <ui-svg class="token-icon" data-src="/images/cryptos/${option.icon || ""}" />
                            <span class="token-symbol">${option.symbol || option.value}</span>
                        </label>
                    </div>
                `
            }
            return html`
                <div class="picker-row">
                    <input id="pick-${option.value}" type="radio" name="${name}" value="${option.value}" ${option.value === this.selected ? "checked" : ""} />
                    <label for="pick-${option.value}" data-search="${option.label || ""}">
                        ${option.label}
                    </label>
                </div>
            `
        })

        render(options, this.modal)
    }
}

customElements.define("ui-picker", PICKER)

export default PICKER
