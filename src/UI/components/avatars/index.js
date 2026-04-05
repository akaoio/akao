import template from "./template.js"
import { Access } from "/core/Access.js"
import Events from "/core/Events.js"
import { render } from "/core/UI.js"
import logic from "./logic.js"

export class AVATARS extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.events = new Events()
        this.subscriptions = []
        this._previewId = null
        this.step = 5
    }

    get id() {
        return logic.id()
    }

    set id(value) {
        return logic.setid(value, this.step, this.total)
    }

    get total() {
        return Number(typeof this.dataset.total !== "undefined" ? this.dataset.total : logic.total() || this.step)
    }

    set total(value) {
        return logic.settotal(value)
    }

    connectedCallback() {
        this.$identicons = this.shadowRoot.querySelector("ui-identicons")

        const seed = async () => {
            const s = await logic.seed()
            if (s != null) this.$identicons.dataset.seed = s
        }

        this.subscriptions.push(
            this.$identicons.events.on("select", ({ detail: { id } }) => {
                this._previewId = id
                this.$identicons.id = id
                this.events.emit("preview", { id })
            }),
            this.$identicons.events.on("increase", () => {
                this.total += this.step
            }),
            Access.on("authenticated", async ({ value }) => {
                this.style.display = value ? "flex" : "none"
                this._previewId = null
                if (value) {
                    await seed()
                    this.render()
                } else this.$identicons.clear()
            }),
            Access.on("avatar", () => this.render())
        )
        if (Access.get("authenticated")) seed().then(() => this.render())
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }

    commit() {
        if (this._previewId === null) return
        logic.setid(this._previewId, this.step, this.total)
        this._previewId = null
    }

    revert(originalId) {
        this._previewId = null
        this.$identicons.id = originalId
    }

    render() {
        this.$identicons.savedId = this.id
        this.$identicons.id = this._previewId ?? this.id
        this.$identicons.total = this.total
    }
}

customElements.define("ui-avatars", AVATARS)
export default AVATARS
