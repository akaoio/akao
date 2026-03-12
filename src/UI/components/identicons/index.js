import template from "./template.js"
import { html, render } from "/core/UI.js"
import Events from "/core/Events.js"

export class IDENTICONS extends HTMLElement {
    constructor() {
        super()
        this.events = new Events()
        this.$subs = []
        this.subscriptions = []
        this.$id = 0
        this.$total = 0
        this.$renderPending = false
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }

    get name() { return this.dataset.name || "item" }
    get size() { return Number(this.dataset.size) || 5 }
    get step() { return Number(this.dataset.step) || 5 }

    static get observedAttributes() {
        return ["data-seed"]
    }

    attributeChangedCallback(name, last, value) {
        if (name === "data-seed" && last !== value) {
            this.clear()
            this.render()
        }
    }

    connectedCallback() {
        this.$container = this.shadowRoot.querySelector("#container")
        const increase = () => this.events.emit("increase")
        const decrease = () => this.events.emit("decrease")
        const $increase = this.shadowRoot.querySelector("#increase")
        const $decrease = this.shadowRoot.querySelector("#decrease")
        $increase.addEventListener("click", increase)
        $decrease.addEventListener("click", decrease)
        this.subscriptions.push(
            () => $increase.removeEventListener("click", increase),
            () => $decrease.removeEventListener("click", decrease)
        )
    }

    disconnectedCallback() {
        this.subscriptions.forEach(off => off())
    }

    set id(value) {
        this.$id = Number(value)
        const input = this.$container?.querySelector(`input#i${this.$id}`)
        if (input) input.checked = true
    }

    set total(value) {
        this.$total = Number(value)
        this.render()
    }

    async create() {
        if (this.$container.children.length >= this.$total) return
        const templates = []
        for (let id = this.$container.children.length; id < this.$total; id++) {
            const seed = await globalThis.sea.work(this.dataset.seed, id)
            const select = () => this.events.emit("select", { id })
            templates.push(html`
                <span class="item">
                    <input id="i${id}" type="radio" name="${this.name}" value="${id}" />
                    <label
                        for="i${id}"
                        ${({ element }) => {
                            element.addEventListener("click", select)
                            this.$subs.push(() => element.removeEventListener("click", select))
                        }}>
                        <ui-identicon data-size="${this.size}" data-seed="${seed}" />
                    </label>
                </span>
            `)
        }
        render(templates, this.$container, { append: true })
        const input = this.$container.querySelector(`input#i${this.$id}`)
        if (input) input.checked = true
    }

    remove() {
        const count = this.$container.children.length
        const min = Math.max(this.step, this.$id + 1)
        if (count <= min) return
        for (let i = 0; i < Math.min(this.step, count - this.$total); i++) this.$container.removeChild(this.$container.lastChild)
        if (this.$container.children.length > this.$total) this.remove()
    }

    clear() {
        this.$subs.forEach(off => off())
        this.$subs = []
        if (!this.$container) return
        while (this.$container.firstChild) this.$container.removeChild(this.$container.firstChild)
    }

    render() {
        if (this.$renderPending) return
        this.$renderPending = true
        queueMicrotask(async () => {
            this.$renderPending = false
            if (!this.$total || !this.dataset.seed || !this.$container) return
            if (this.$container.children.length < this.$total) await this.create()
            if (this.$container.children.length > this.$total) this.remove()
        })
    }
}

customElements.define("ui-identicons", IDENTICONS)
export default IDENTICONS
