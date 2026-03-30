import { Elements } from "/core/Stores.js"
import { render } from "/core/UI.js"
import template from "./template.js"
import States from "/core/States.js"
import { Access } from "/core/Access.js"

export class AUTHORIZE extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
        this.states = new States({ state: "listening" })
        this.pending = null
        this.render = this.render.bind(this)
        this.toggle = this.toggle.bind(this)
        this.wave = this.wave.bind(this)
        this.grant = this.grant.bind(this)
        this.deny = this.deny.bind(this)
        this.stop = this.stop.bind(this)
        this.onclose = this.onclose.bind(this)
    }

    connectedCallback() {
        this.$authorize = this.shadowRoot.querySelector("#authorize")
        this.$modal = this.shadowRoot.querySelector("ui-modal")
        this.$wave = this.shadowRoot.querySelector("ui-wave")
        this.$epub = this.shadowRoot.querySelector("#epub")
        this.$grant = this.shadowRoot.querySelector("#grant")
        this.$deny = this.shadowRoot.querySelector("#deny")
        this.$stop = this.shadowRoot.querySelector("#stop")
        this.$dialog = this.$modal.shadowRoot?.querySelector("dialog")
        this.$authorize.addEventListener("click", this.toggle)
        this.$grant.addEventListener("click", this.grant)
        this.$deny.addEventListener("click", this.deny)
        this.$stop.addEventListener("click", this.stop)
        this.$dialog?.addEventListener("close", this.onclose)
        this.subscriptions.push(
            () => this.$deny.removeEventListener("click", this.deny),
            () => this.$grant.removeEventListener("click", this.grant),
            () => this.$stop.removeEventListener("click", this.stop),
            () => this.$dialog?.removeEventListener("close", this.onclose),
            this.$wave.events.on("message", this.wave),
            this.states.on("state", this.render)
        )
        this.render()
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
        this.$wave?.stop?.()
    }

    onclose() {
        this.$wave?.stop?.()
        this.pending = null
        this.states.set({ state: "listening" })
    }

    async wave(event) {
        const { parsed } = event?.detail || {}
        if (!parsed || typeof parsed !== "object") return
        if (parsed[":"] !== ">" || !parsed["~"]) return
        if (this.states.get("state") !== "listening") return
        this.pending = parsed
        const epub = parsed["~"]
        this.$epub.textContent = !epub || epub.length <= 12 ? epub || "" : `${epub.slice(0, 5)}...${epub.slice(-5)}`
        this.states.set({ state: "confirm" })
    }

    async deny() {
        if (!this.$wave) return
        this.states.set({ state: "sending" })
        await this.$wave.send({ ":": "!>" })
        this.pending = null
        this.states.set({ state: "listening" })
        if (this.$dialog?.open) this.$wave.listen()
    }

    stop() {
        this.$wave?.stop?.()
        this.pending = null
        this.states.set({ state: "listening" })
    }

    async grant() {
        if (!this.pending) return
        const { sea } = globalThis
        const pair = Access.get("pair")
        const seed = Access.get("seed")
        if (!Access.get("authenticated") || !pair || !seed || !sea?.secret || !sea?.encrypt) return
        const secret = await sea.secret(this.pending["~"], pair)
        // Uint8Array does not survive JSON serialization — convert to plain Array first
        const seedData = seed instanceof Uint8Array ? Array.from(seed) : seed
        const encrypted = await sea.encrypt(seedData, secret, null, { raw: true })
        const payload = { "~": pair.epub, "!": encrypted.ct, "@": encrypted.iv, "#": encrypted.s }
        this.states.set({ state: "sending" })
        await this.$wave.send(payload)
        this.pending = null
        this.states.set({ state: "listening" })
        if (this.$dialog?.open) this.$wave.listen()
    }

    toggle() {
        const check = Elements.Access?.checkpoint()
        if (!check) return
        if (!this.$dialog?.open) this.$wave.listen()
        this.$modal.toggleModal()
    }

    render() {
        const state = this.states.get("state")
        this.$grant.hidden = state !== "confirm"
        this.$deny.hidden = state !== "confirm"
        this.$stop.hidden = state !== "sending"
    }
}

customElements.define("ui-authorize", AUTHORIZE)
export default AUTHORIZE
