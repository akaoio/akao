import { Context } from "/core/Context.js"
import States from "/core/States.js"

export class CONTEXT extends HTMLElement {
    constructor(props = {}) {
        super()
        this.states = new States({ key: props.key || null })
        this.subscriptions = []
        this.subscription = null
        this.render = this.render.bind(this)
        this.on = this.on.bind(this)
        this.off = this.off.bind(this)
    }

    static get observedAttributes() {
        return ["data-key", "data-fallback"]
    }

    attributeChangedCallback(name, last, value) {
        if (!["data-key", "data-fallback"].includes(name) || last === value) return
        this.off()
        this.on()
        this.states.set({ key: value })
    }

    connectedCallback() {
        this.subscriptions.push(this.states.on("key", this.render))
        this.on()
        this.render()
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
        this.off()
    }

    on() {
        if (!this.dataset.key) return
        const key = this.dataset.key.split(".")
        this.subscription = Context.on(key, this.render)
    }

    off() {
        this.subscription?.()
    }

    render() {
        let key = this.states.get("key") || this.dataset.key
        if (!key) return
        if (typeof key === "string") key = key.split(".")
        this.innerText = Context.get(key) || key.at(-1) || this.dataset.fallback || ""
    }
}

customElements.define("ui-context", CONTEXT)

export default CONTEXT
