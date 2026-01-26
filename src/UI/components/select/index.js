import States from "/core/States.js"
import { render } from "/core/UI.js"
import { template } from "./template.js"

export class SELECT extends HTMLElement {
    constructor() {
        super()
        this.states = new States({ options: [], selected: null })
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
        this.render = this.render.bind(this)
    }

    static get observedAttributes() {
        return ["data-name", "data-placeholder", "data-required", "data-selected"]
    }

    attributeChangedCallback(name, last, value) {
        if (last === value) return
        if (name === "data-name") this.select.setAttribute("name", value)
        else if (name === "data-placeholder") this.placeholder.dataset.key = value
        else if (name === "data-required") {
            if (value !== null) this.select.setAttribute("required", "")
            else this.select.removeAttribute("required")
        }
        if (name === "data-selected") {
            this.states.set({ selected: value })
            this.select.value = value
        }
    }

    connectedCallback() {
        this.select = this.shadowRoot.querySelector("select")
        this.placeholder = this.shadowRoot.querySelector("#placeholder")
        if (this.dataset.required) this.select.setAttribute("required", "")
        if (this.dataset.name) this.select.setAttribute("name", this.dataset.name)
        if (this.dataset.placeholder) this.placeholder.dataset.key = this.dataset.placeholder
        this.subscriptions.push(this.states.on("options", this.render))
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }

    render() {
        this.select.querySelectorAll("option:not([disabled])").forEach(option => option.remove())
        const options = this.states.get("options").map(option => html`
            <option value="${option.value}" ${option.value === this.states.get("selected") ? "selected" : ""}>
                ${option.label}
            </option>
        `)
        render(options, this.select, { append: true })
    }
}

customElements.define("ui-select", SELECT)

export default SELECT