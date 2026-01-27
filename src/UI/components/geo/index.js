import { html, render } from "/core/UI.js"
import States from "/core/States.js"
import DB from "/core/DB.js"
import SELECT from "/UI/components/select/index.js"

export class GEO extends HTMLElement {
    constructor() {
        super()
        this.states = new States({ countries: [], country: null })
        this.attachShadow({ mode: "open" })
        this.render = this.render.bind(this)
    }

    static get observedAttributes() {
        return ["data-country", "data-adm1", "data-adm2", "data-adm3", "data-adm4", "data-adm5"]
    }

    attributeChangedCallback(name, old, value) {
        if (old === value) return
        const key = name.replace("data-", "")
        this.states.set({ [key]: value || null })
        this.render()
    }

    async connectedCallback() {
        const countries = await DB.get(["geo", "countries.json"])
        if (countries) {
            this.states.set({ countries: Object.values(countries).sort((a, b) => a.name.localeCompare(b.name)) })
        }
        this.render()
    }

    render() {
        if (!this.states.get("countries")?.length) return
        const countries = new SELECT({ 
            name: "country", 
            options: this.states.get("countries")?.map(country => ({
                value: country.code,
                label: country.name
            })),
            placeholder: "dictionary.country"
        })
        this.shadowRoot.appendChild(countries)
    }
}

customElements.define("ui-geo", GEO)

export default GEO