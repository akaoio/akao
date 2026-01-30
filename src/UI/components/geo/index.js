import { html, render } from "/core/UI.js"
import States from "/core/States.js"
import DB from "/core/DB.js"
import SELECT from "/UI/components/select/index.js"
import template from "./template.js"

export class GEO extends HTMLElement {
    constructor() {
        super()
        this.states = new States({ id: null, countries: [], country: null, current: null })
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.render = this.render.bind(this)
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
        this.render()
    }

    async render() {
        if (!this.states.get("id")) return
        const id = DB.path(this.states.get("id"))
        const data = await DB.get(["geo", ...id.with(-1, `${id.at(-1)}.json`)])
        console.log(data)
        if (data?.children.length) {
            let options = []
            for (const child of data.children) {
                const $id = DB.path(child)
                const $data = await DB.get(["geo", ...$id.with(-1, `${$id.at(-1)}.json`)])
                if ($data) options.push({ value: $data.id, label: $data.name })
            }
            const select = new SELECT({
                name: "", 
                options, 
                placeholder: "dictionary.area", 
                selected: null, 
                change: event => this.states.set({ id: Number(event.target.value), current: select })
            })
            while (this.states.get("current")?.nextSibling) this.states.get("current")?.nextSibling.remove()
            this.states.get("current").after(select)
        }
        if (data?.parent) {
            console.log("Parent:", data.parent)
        }
    }
}

customElements.define("ui-geo", GEO)

export default GEO