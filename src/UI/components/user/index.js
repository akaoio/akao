import { Elements } from "/core/Stores.js"
import { Access } from "/core/Access.js"
import template from "./template.js"
import { render } from "/core/UI.js"

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
            Access.on("authenticated", () => {
                if (!Access.get("authenticated")) return this.identicon.removeAttribute("data-seed")
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
        if (Access.get("avatar")?.id == null) return
        const { sea } = globalThis
        if (!sea) return
        if (!Access.get("seed")) return
        const namespaced = await sea.work(Access.get("seed"), "avatar")
        const seed = await sea.work(namespaced, Access.get("avatar").id)
        this.identicon.dataset.seed = seed
    }
}

customElements.define("ui-user", USER)

export default USER
