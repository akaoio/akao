import { render } from "/core/UI.js"
import States from "/core/States.js"
import SELECT from "/UI/components/select/index.js"
import BaseElement from "/UI/BaseElement.js"
import template from "./template.js"
import logic from "./logic.js"

export class GEO extends BaseElement {
    constructor() {
        super()
        this.states = new States({ id: null, country: null, current: null })
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.create = this.create.bind(this)
        this.render = this.render.bind(this)
        this.clear = this.clear.bind(this)
        this.reset = this.reset.bind(this)
    }

    static get observedAttributes() {
        return ["data-id"]
    }

    attributeChangedCallback(name, last, value) {
        if (last === value) return
        this.states.set({ [name.replace("data-", "")]: Number(value) || null })
        this.render()
    }

    async onConnect() {
        const country = this.shadowRoot.querySelector("#country")
        country.states.set({ options: await logic.countries() })
        country.props.change = event => this.states.set({ id: Number(event.target.value), current: country })
        if (!this.states.get("id") && this.dataset.id) this.states.set({ id: Number(this.dataset.id) })
        this.watch(this.states, "id", this.render)
    }

    async create({ id, selected } = {}) {
        if (!id) return
        const data = await logic.node(id)
        if (data?.children?.length) {
            const options = await logic.children(id)
            const select = new SELECT({
                name: `level-${data.level + 1}`,
                options,
                placeholder: "dictionary.area",
                selected,
                required: true,
                change: event => this.states.set({ id: Number(event.target.value), current: select })
            })
            return select
        }
    }

    clear() {
        this.states.set({ id: null, country: null, current: null })
    }

    reset() {
        if (!this.dataset.id) this.clear()
        if (this.dataset.id !== this.states.get("id")) this.states.set({ id: Number(this.dataset.id) })
    }

    async render() {
        if (!this.states.get("id")) {
            const country = this.shadowRoot.querySelector("#country")
            country.states.set({ selected: null })
            country.select.selectedIndex = 0
            this.shadowRoot.querySelectorAll("ui-select:not([id='country'])").forEach(select => select.remove())
            return
        }
        while (this.states.get("current")?.nextSibling) this.states.get("current")?.nextSibling.remove()
        const data = await logic.node(this.states.get("id"))
        if (data?.children.length) {
            const select = await this.create({ id: this.states.get("id") })
            this.states.get("current").after(select)
        }
        if (data?.parent) {
            let parent = this.states.get("current")?.previousElementSibling
            if (!parent || Number(parent.states.get("selected")) !== Number(data.parent)) {
                let $data = data
                let current
                while ($data?.parent) {
                    const $parent = await logic.node($data.parent)
                    if ($parent) {
                        const element = await this.create({ id: $parent.id, selected: $data.id })
                        if (this.shadowRoot.querySelectorAll("ui-select:not([id='country'])").length === 0) this.shadowRoot.appendChild(element)
                        else if (current) current.before(element)
                        current = element
                        if ($data?.parent) $data = $parent
                    } else break
                }
                this.shadowRoot.querySelector("#country").dataset.selected = $data.id
            }
        }
    }
}

customElements.define("ui-geo", GEO)

export default GEO