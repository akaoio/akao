import { Elements } from "/core/Stores.js"
import { Access } from "/core/Access.js"
import template from "./template.js"
import { render } from "/core/UI.js"
import logic from "./logic.js"

export class USER extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
        this.toggle = this.toggle.bind(this)
        this.render = this.render.bind(this)
    }

    get identicon() {
        return this.shadowRoot.querySelector("ui-identicon")
    }

    connectedCallback() {
        this.shadowRoot.querySelector(".user").addEventListener("click", this.toggle)
        this.subscriptions.push(
            () => this.shadowRoot.querySelector(".user").removeEventListener("click", this.toggle),
            Access.on("authenticated", ({ value }) => {
                if (!value) this.identicon.removeAttribute("data-seed")
            }),
            Access.on("avatar", this.render)
        )
        if (Access.get("authenticated")) this.render()
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }

    toggle() {
        const check = Elements.Access?.checkpoint()
        if (!check) return
        Elements.Profile.toggle()
    }

    async render() {
        const seed = await logic.seed()
        if (seed != null) this.identicon.dataset.seed = seed
    }
}

customElements.define("ui-user", USER)

export default USER
