import { html, render } from "/core/UI.js"
import States from "/core/States.js"
import DB from "/core/DB.js"
import SELECT from "/UI/components/select/index.js"
import template from "./template.js"

export class GEO extends HTMLElement {
    constructor() {
        super()
        this.states = new States({ id: null, countries: [], country: null })
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.render = this.render.bind(this)
        this.create = this.create.bind(this)
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
        const countries = await DB.get(["geo", "countries.json"])
        if (countries) this.states.set({ countries: Object.values(countries).sort((a, b) => a.name.localeCompare(b.name)).map(country => ({ value: country.id, label: country.name }))})
        if (!this.states.get("id") && this.dataset.id) this.states.set({ id: this.dataset.id || null })
        this.subscriptions.push(this.states.on("id", this.render))
        this.render()
    }

    async create({ id, name, options = [], placeholder, selected } = {}) {
        const path = id ? DB.path(id) : ["countries"]
        const data = await DB.get(["geo", ...path.with(-1, `${path.at(-1)}.json`)])
        let children = []
        children = Array.isArray(data) ? data : data?.children || []
        for (const child of children) {
            if (typeof child === "object") options.push({ value: child.id, label: child.name })
            else {
                const $id = DB.path(child)
                const $child = await DB.get(["geo", ...$id.with(-1, `${$id.at(-1)}.json`)])
                if ($child) options.push({ value: $child.id, label: $child.name })
            }
        }
        const select = new SELECT({
            name, 
            options, 
            placeholder, 
            selected, 
            change: event => {
                const value = event.target.value
                this.create({id:value})
            }
        })
        render(select, this.shadowRoot, { append: true })
        return select
    }

    render() {
        // if (!this.states.get("countries")?.length) return
        // const countries = new SELECT({ 
        //     name: "country", 
        //     options: this.states.get("countries")?.map(country => ({
        //         value: country.code,
        //         label: country.name
        //     })),
        //     placeholder: "dictionary.country"
        // })
        // this.create()
        // this.shadowRoot.appendChild(countries)
        // render(geo, this.shadowRoot, { append: true })
        // console.log("RUNDER", this.states.get("countries"))
        this.shadowRoot.querySelector("#country").states.set({ options: this.states.get("countries") })
    }
}

customElements.define("ui-geo", GEO)

export default GEO