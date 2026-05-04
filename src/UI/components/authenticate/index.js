import { render } from "/core/UI.js"
import template from "./template.js"
import Events from "/core/Events.js"
import States from "/core/States.js"
import { Context } from "/core/Context.js"
import { notify } from "/core/Utils/browser.js"
import Component from "/core/UI/Component.js"
import Logic from "./logic.js"

export class AUTHENTICATE extends Component {
    static module = import.meta.url
    constructor() {
        super()
        this.template = template // Store for HMR
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.events = new Events(this)
        this.logic = new Logic()
        this.states = new States({ method: null })
        this.state = "neutral"
        this.onwave = this.onwave.bind(this)
        this.onrequestbtn = this.onrequestbtn.bind(this)
        this.onstopbtn = this.onstopbtn.bind(this)
        this.passkey = this.passkey.bind(this)
        this.stop = this.stop.bind(this)
        this.render = this.render.bind(this)
        this.wave = this.wave.bind(this)
    }

    onconnect() {
        this.$wave = this.shadowRoot.querySelector("ui-wave")
        this.$requestbtn = this.shadowRoot.querySelector("#request-btn")
        this.$stopbtn = this.shadowRoot.querySelector("#stop-btn")
        this.$pub = this.shadowRoot.querySelector("#pub")
        this.listen(this.$requestbtn, "click", this.onrequestbtn)
        this.listen(this.$stopbtn, "click", this.onstopbtn)
        this.listen(this.shadowRoot.querySelector("#passkey"), "click", this.passkey)
        this.listen(this.shadowRoot.querySelector("#wave"), "click", this.wave)
        this.sub(
            this.$wave.events.on("message", this.onwave),
            this.states.on("method", this.render)
        )
        this.setstate("neutral")
        this.initpair()
    }

    ondisconnect() {
        this.stop()
    }

    async initpair() {
        const pair = await this.logic.pair()
        if (!pair || !this.$pub) return
        this.$pub.textContent = this.logic.abbrev(pair.pub)
    }

    setstate(state) {
        this.state = state
        this.$requestbtn.hidden = state !== "neutral"
        this.$stopbtn.hidden = state === "neutral"
    }

    done(response) {
        this.events.emit("done", { response }, { bubbles: true, composed: true })
    }

    async onwave(event) {
        const { parsed, message } = event?.detail || {}
        const result = await this.logic.handle(parsed ?? message)
        if (!result) return
        if (result.type === "deny") {
            notify({ content: Context.get(["dictionary", "accessDenied"]) })
            return
        }
        if (result.type === "grant") {
            this.stop()
            this.logic.wave(result.seed)
                .then((response) => this.done(response))
                .catch(() => {})
        }
    }

    onrequestbtn() {
        this.setstate("listening")
        this.logic.request(this.$wave).catch((error) => {
            this.setstate("neutral")
            if (error?.message) notify({ content: error.message })
        })
    }

    onstopbtn() {
        this.stop()
    }

    passkey() {
        this.states.set({ method: "passkey" })
        this.logic.passkey().then((response) => this.done(response))
    }

    wave() {
        this.states.set({ method: "wave" })
    }

    stop() {
        this.logic.reset()
        this.$wave?.stop?.()
        this.setstate("neutral")
        this.initpair()
    }

    reset() {
        this.states.set({ method: null })
    }

    render() {
        const method = this.states.get("method")
        if (method !== "wave") this.stop()
        this.shadowRoot.querySelector("#methods-screen").hidden = method !== null
        this.shadowRoot.querySelector("#passkey-screen").hidden = method !== "passkey"
        this.shadowRoot.querySelector("#wave-screen").hidden = method !== "wave"
    }
}

customElements.define("ui-authenticate", AUTHENTICATE)
export default AUTHENTICATE
