import { render } from "/core/UI.js"
import States from "/core/States.js"
import DB from "/core/DB.js"
import SELECT from "/UI/components/select/index.js"
import template from "./template.js"

export class GEO extends HTMLElement {
    constructor() {
        super()
        this.states = new States({ id: null, country: null, current: null })
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.create = this.create.bind(this)
        this.render = this.render.bind(this)
        this.clear = this.clear.bind(this)
        this.subscriptions = []
    }

    static get observedAttributes() {
        return ["data-id"]
    }

    attributeChangedCallback(name, old, value) {
        if (old === value) return
        this.states.set({ [name.replace("data-", "")]: Number(value) || null })
        this.render()
    }

    async connectedCallback() {
        const data = await DB.get(["geo", "countries.json"])
        const country = this.shadowRoot.querySelector("#country")
        country.states.set({ options: Object.values(data).sort((a, b) => a.name.localeCompare(b.name)).map(country => ({ value: country.id, label: country.name })) })
        country.props.change = event => this.states.set({ id: Number(event.target.value), current: country })
        if (!this.states.get("id") && this.dataset.id) this.states.set({ id: Number(this.dataset.id) })
        this.subscriptions.push(this.states.on("id", this.render))
    }

    async create({ id, selected } = {}) {
        if (!id) return
        const path = DB.path(id)
        const data = await DB.get(["geo", ...path.with(-1, `${path.at(-1)}.json`)])
        if (data?.children?.length) {
            const options = []
            for (const child of data.children) {
                const $id = DB.path(child)
                const $data = await DB.get(["geo", ...$id.with(-1, `${$id.at(-1)}.json`)])
                if ($data) options.push({ value: $data.id, label: $data.name })
            }
            const select = new SELECT({
                name: `level-${data.level + 1}`,
                options,
                placeholder: "dictionary.area",
                selected,
                change: event => this.states.set({ id: Number(event.target.value), current: select })
            })
            return select
        }
    }

    clear() {
        this.states.set({ id: null, country: null, current: null })
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
        const id = DB.path(this.states.get("id"))
        const data = await DB.get(["geo", ...id.with(-1, `${id.at(-1)}.json`)])
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
                    const $id = DB.path($data.parent)
                    const $parent = await DB.get(["geo", ...$id.with(-1, `${$id.at(-1)}.json`)])
                    if ($parent) {
                        const element = await this.create({ id: $parent.id, selected: $data.id })
                        if (this.shadowRoot.querySelectorAll("ui-select:not([id='country'])").length === 0) this.shadowRoot.appendChild(element)
                        else if (current) current.before(element)
                        current = element
                        if ($data?.parent) $data = $parent
                    }
                }
                this.shadowRoot.querySelector("#country").dataset.selected = $data.id
            }
        }
    }
}

customElements.define("ui-geo", GEO)

export default GEO