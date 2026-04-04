import template from "./template.js"
import States from "/core/States.js"
import ITEM from "/UI/components/item/index.js"
import { render } from "/core/UI.js"
import logic from "./logic.js"

export class ITEMS extends HTMLElement {
    constructor() {
        super()
        this.states = new States()
        this.attachShadow({ mode: "open" })
        this.render = this.render.bind(this)
        render(template, this.shadowRoot)
    }

    async connectedCallback() {
        this.states.on("pages", this.render)
        const data = await logic.meta()
        if (data) this.states.set(data)
    }

    async render() {
        const pages = this.states.get("pages")
        const start = this.dataset.start ? parseInt(this.dataset.start) : 1
        let end = this.dataset.end ? parseInt(this.dataset.end) : pages
        const data = await logic.items(start, end, pages)
        const children = data.map((item) => {
            const element = new ITEM()
            element.dataset.key = item
            return element
        })
        this.shadowRoot.querySelector("#items").replaceChildren(...children)
    }
}

customElements.define("ui-items", ITEMS)

export default ITEMS
