import { Elements } from "/core/Stores.js"
import { render } from "/core/UI.js"
import template from "./template.js"
import States from "/core/States.js"
import BaseElement from "/UI/BaseElement.js"
import logic from "./logic.js"

export class AUTHORIZE extends BaseElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
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

    onConnect() {
        this.$authorize = this.shadowRoot.querySelector("#authorize")
        this.$modal = this.shadowRoot.querySelector("ui-modal")
        this.$wave = this.shadowRoot.querySelector("ui-wave")
        this.$epub = this.shadowRoot.querySelector("#epub")
        this.$grant = this.shadowRoot.querySelector("#grant")
        this.$deny = this.shadowRoot.querySelector("#deny")
        this.$stop = this.shadowRoot.querySelector("#stop")
        this.$dialog = this.$modal.shadowRoot?.querySelector("dialog")
        this.listen(this.$authorize, "click", this.toggle)
        this.listen(this.$grant, "click", this.grant)
        this.listen(this.$deny, "click", this.deny)
        this.listen(this.$stop, "click", this.stop)
        this.listen(this.$dialog, "close", this.onclose)
        this.subscribe(
            this.$wave.events.on("message", this.wave),
            this.states.on("state", this.render)
        )
        this.render()
    }

    onDisconnect() {
        this.$wave?.stop?.()
    }

    onclose() {
        this.$wave?.stop?.()
        this.pending = null
        this.states.set({ state: "listening" })
    }

    async wave(event) {
        const { parsed } = event?.detail || {}
        if (!logic.isrequest(parsed)) return
        if (this.states.get("state") !== "listening") return
        this.pending = parsed
        this.$epub.textContent = logic.epub(parsed["~"])
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
        const payload = await logic.encode(this.pending["~"])
        if (!payload) return
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
