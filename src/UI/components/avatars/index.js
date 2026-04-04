import template from "./template.js"
import { Access, setAvatar } from "/core/Access.js"
import { render } from "/core/UI.js"

export class AVATARS extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
        this.step = 5
    }

    get id() {
        return Number(Access.get("avatar")?.id || 0)
    }

    set id(value) {
        value = Number(value)
        const total = value >= this.total ? Math.ceil((value + 1) / this.step) * this.step : this.total
        setAvatar({ id: value, total: total !== this.total ? total : undefined })
        return value
    }

    get total() {
        return Number(typeof this.dataset.total !== "undefined" ? this.dataset.total : Access.get("avatar")?.total || this.step)
    }

    set total(value) {
        return setAvatar({ total: Number(value) })
    }

    connectedCallback() {
        this.$identicons = this.shadowRoot.querySelector("ui-identicons")

        const seed = async () => {
            if (!Access.get("seed")) return
            const hashed = await globalThis.sea.work(Access.get("seed"), "avatar")
            this.$identicons.dataset.seed = hashed
        }

        this.subscriptions.push(
            this.$identicons.events.on("select", ({ detail: { id } }) => { this.id = id }),
            this.$identicons.events.on("increase", () => { this.total += this.step }),
            Access.on("authenticated", async ({ value }) => {
                this.style.display = value ? "flex" : "none"
                if (value) { await seed(); this.render() }
                else this.$identicons.clear()
            }),
            Access.on("avatar", () => this.render())
        )
        if (Access.get("authenticated")) seed().then(() => this.render())
    }

    disconnectedCallback() {
        this.subscriptions.forEach(off => off())
    }

    render() {
        this.$identicons.id = this.id
        this.$identicons.total = this.total
    }
}

customElements.define("ui-avatars", AVATARS)
export default AVATARS
