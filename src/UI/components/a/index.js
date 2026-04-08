import { Context } from "/core/Context.js"
import logic from "./logic.js"

export class A extends HTMLAnchorElement {
    constructor() {
        super()
        this.click = this.click.bind(this)
        this.render = this.render.bind(this)
        this.subscriptions = []
    }

    static get observedAttributes() {
        return ["data-to", "data-locale"]
    }

    attributeChangedCallback(name, last, value) {
        if (last === value) return
        this.render()
    }

    connectedCallback() {
        this.addEventListener("click", this.click)
        this.subscriptions.push(Context.on("locale", this.render))
    }

    disconnectedCallback() {
        this.removeEventListener("click", this.click)
        this.subscriptions.forEach((off) => off())
    }

    click(e) {
        e.preventDefault()
        logic.navigate(this.getAttribute("href"))
    }

    render() {
        const to = this.dataset.to || this.getAttribute("href")
        const locale = this.dataset.locale || Context.get("locale").code
        const path = logic.href(to, locale)
        if (this.getAttribute("href") !== path) this.setAttribute("href", path)
    }
}

customElements.define("ui-a", A, { extends: "a" })
