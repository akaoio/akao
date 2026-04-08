import template from "./template.js"
import States from "/core/States.js"
import { html, render } from "/core/UI.js"
import BaseElement from "/UI/BaseElement.js"

export class PICKER extends BaseElement {
    constructor() {
        super()
        this.states = new States({ options: [], selected: null })
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.show = this.show.bind(this)
        this.close = this.close.bind(this)
        this.select = this.select.bind(this)
        this.render = this.render.bind(this)
    }

    static get observedAttributes() {
        return ["data-name", "data-selected"]
    }

    attributeChangedCallback(name, last, value) {
        if (last === value) return
        this.states.set({ [name.replace("data-", "")]: value })
    }

    onConnect() {
        this.modal = this.shadowRoot.querySelector("ui-modal")
        this.modal.dataset.header = this.dataset.header
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
        if (typeof this.callback == "function") this.callback(value)
    }

    render() {
        const name = this.states.get("name") || this.dataset.name

        // Create single template with all options
        const options = this.states
            .get("options")
            .map((option) => {
                return html`
                    <input id="${option.value}" type="radio" name="${name}" value="${option.value}" ${option.value == this.selected ? "checked" : ""} />
                    <label for="${option.value}">
                        ${option.label}
                    </label>
                `
            })
        render(options, this.modal)
    }
}

customElements.define("ui-picker", PICKER)

export default PICKER
