import States from "/core/States.js"
import { html, render } from "/core/UI.js"
import Component from "/core/UI/Component.js"
import { template } from "./template.js"

export class SELECT extends Component {
    static module = import.meta.url

    constructor(props = {}) {
        super()
        this.props = props || {}
        this.states = new States({ options: props?.options || [], selected: props?.selected || null })
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.change = this.change.bind(this)
        this.render = this.render.bind(this)
        this.renderSelected = this.renderSelected.bind(this)
    }

    static get observedAttributes() {
        return ["data-name", "data-required", "data-selected"]
    }

    attributeChangedCallback(name, last, value) {
        if (last === value) return
        this.select = this.select || this.shadowRoot.querySelector("select")
        if (name === "data-name") this.select.setAttribute("name", value)
        else if (name === "data-required")
            if (value !== null) this.select.setAttribute("required", "required")
            else this.select.removeAttribute("required")

        if (name === "data-selected") {
            this.states.set({ selected: value })
            this.select.value = value
        }
    }

    onconnect() {
        this.select = this.select || this.shadowRoot.querySelector("select")
        this.select.setAttribute("name", this.props.name || this.dataset.name)
        if (this.dataset.required || this.props.required) this.select.setAttribute("required", "required")
        if (this.dataset.name) this.select.setAttribute("name", this.dataset.name)
        this.listen(this.select, "change", this.change)
        this.watch(this.states, "options", this.render)
        this.watch(this.states, "selected", this.renderSelected)
        this.render()
        this.renderSelected()
    }

    renderSelected() {
        this.toggleAttribute("data-has-value", !!this.states.get("selected"))
    }

    change(event) {
        this.states.set({ selected: event.target.value })
        if (this.props.change) this.props.change(event)
    }

    render() {
        this.select.querySelectorAll("option").forEach((option) => option.remove())
        const options = this.states.get("options").map(
            (option) => html`
                <option value="${option.value}" ${option.value === this.states.get("selected") ? "selected" : ""}>${option.label}</option>
            `
        )
        render(options, this.select, { append: true })
    }
}

customElements.define("ui-select", SELECT)

export default SELECT
