import template from "./template.js"
import { Context, setTheme } from "/core/Context.js"
import { render } from "/core/UI.js"
import logic from "./logic.js"

export class THEMES extends HTMLElement {
    static module = import.meta.url
    constructor() {
        super()
        this.template = template // Store for HMR
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }

    connectedCallback() {
        const button = this.shadowRoot.querySelector("button")
        button.classList.add(Context.get("theme"))
        button.addEventListener("click", () => setTheme(logic.toggle(Context.get("theme"))))
        this.subscription = Context.on("theme", ({ value, last }) => {
            button.classList.remove(last)
            button.classList.add(value)
        })
    }

    disconnectedCallback() {
        this.subscription.off()
    }
}

customElements.define("ui-themes", THEMES)

export default THEMES
