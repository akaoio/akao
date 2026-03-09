import template from "./template.js"
import { Access, setAvatar } from "/core/Access.js"
import { html, render } from "/core/UI.js"

export class AVATARS extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
        this.step = 5
        this.increase = this.increase.bind(this)
        this.decrease = this.decrease.bind(this)
        this.render = this.render.bind(this)
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
        this.avatars = this.shadowRoot.querySelector("#avatars")
        this.shadowRoot.querySelector("#increase").addEventListener("click", this.increase)
        this.shadowRoot.querySelector("#decrease").addEventListener("click", this.decrease)
        this.subscriptions.push(
            Access.on("authenticated", ({ value }) => {
                this.style.display = value ? "flex" : "none"
                if (!value) while (this.avatars.firstChild) this.avatars.removeChild(this.avatars.firstChild)
            }),
            Access.on("avatar", this.render),
            () => this.shadowRoot.querySelector("#increase").removeEventListener("click", this.increase),
            () => this.shadowRoot.querySelector("#decrease").removeEventListener("click", this.decrease)
        )
        if (Access.get("authenticated")) this.render()
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }

    increase() {
        this.total += this.step
    }

    decrease() {
        if (this.total - this.step > this.id) this.total -= this.step
    }

    async create() {
        if (this.avatars.children.length >= this.total) return
        const templates = []
        for (let id = this.avatars.children.length; id < this.total; id++) {
            const seed = await globalThis.sea.work(Access.get("seed"), id)
            const select = () => this.select({ id })
            templates.push(html`
                <span class="avatar">
                    <input id="i${id}" type="radio" name="avatar" value="${id}" ${id === this.id ? "checked" : ""} />
                    <label
                        for="i${id}"
                        ${({ element }) => {
                            element.addEventListener("click", select)
                            this.subscriptions.push(() => element.removeEventListener("click", select))
                        }}>
                        <ui-identicon data-size="7" data-seed="${seed}" />
                    </label>
                </span>
            `)
        }
        render(templates, this.avatars, { append: true })
    }

    remove() {
        const count = this.avatars.children.length
        const min = Math.max(this.step, this.id + 1)
        if (count <= min) return
        for (let i = 0; i < Math.min(this.step, count - this.total); i++) this.avatars.removeChild(this.avatars.lastChild)
        if (this.avatars.children.length > this.total) this.remove()
    }

    async select({ id }) {
        if (!this.avatars.querySelector(`input#i${id}`)) await this.create()
        this.id = id
    }

    async render() {
        if (!Access.get("authenticated")) return
        if (this.avatars.children.length < this.total) await this.create()
        if (this.avatars.children.length > this.total) this.remove()
    }
}

customElements.define("ui-avatars", AVATARS)
export default AVATARS
