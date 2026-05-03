import template from "./template.js"
import { Access } from "/core/Access.js"
import Events from "/core/Events.js"
import { render } from "/core/UI.js"
import Component from "/core/UI/Component.js"
import logic from "./logic.js"

export class AVATARS extends Component {
    static module = import.meta.url
    constructor() {
        super()
        this.template = template // Store for HMR
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.events = new Events()
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

    onconnect() {
        this.$identicons = this.shadowRoot.querySelector("ui-identicons")

        const seed = async () => {
            const s = await logic.seed()
            if (s != null) this.$identicons.dataset.seed = s
        }

        const $cancel = this.shadowRoot.querySelector("#avatar-cancel")
        const $accept = this.shadowRoot.querySelector("#avatar-accept")
        const onCancel = () => this.events.emit("cancel")
        const onAccept = () => this.events.emit("accept")
        this.listen($cancel, "click", onCancel)
        this.listen($accept, "click", onAccept)

        this.sub(
            this.$identicons.events.on("select", ({ detail: { id } }) => {
                this._previewId = id
                this.$identicons.id = id
                this.$identicons.previewId = id
                this.events.emit("preview", { id })
            }),
            this.$identicons.events.on("increase", ({ detail }) => {
                if (detail?.scrollToNew) this.$identicons.render(true)
                this.total += this.step
                this.$identicons.total = this.total
            }),
            this.$identicons.events.on("decrease", () => {
                if (this.total - this.step > (this._previewId ?? this.id)) {
                    this.total -= this.step
                    this.$identicons.total = this.total
                }
            }),
            Access.on("authenticated", async ({ value }) => {
                this.style.display = value ? "flex" : "none"
                this._previewId = null
                if (value) {
                    await seed()
                    this.render()
                } else this.$identicons.clear()
            })
        )
        if (Access.get("authenticated")) seed().then(() => this.render())
    }

    disconnectedCallback() {
        this.subs.forEach((off) => off())
    }

    scrollToSelected() {
        this.$identicons.scrollTo(this._previewId ?? this.id)
    }

    commit() {
        if (this._previewId === null) return
        logic.setid(this._previewId, this.step, this.total)
        this.$identicons.savedId = this._previewId
        this.$identicons.previewId = null
        this._previewId = null
    }

    revert(originalId) {
        this._previewId = null
        this.$identicons.previewId = null
        this.$identicons.id = originalId
    }

    render() {
        this.$identicons.total = this.total
        this.$identicons.id = this._previewId ?? this.id
        this.$identicons.savedId = this.id
    }
}

customElements.define("ui-avatars", AVATARS)
export default AVATARS
