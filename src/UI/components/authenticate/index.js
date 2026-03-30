import { render } from "/core/UI.js"
import template from "./template.js"
import Events from "/core/Events.js"
import { wave } from "/core/Access.js"
import WaveAuth from "./wave.js"
import signinWithPasskey from "./passkey.js"
import States from "/core/States.js"

export class AUTHENTICATE extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.events = new Events(this)
        this.subscriptions = []
        this.waveauth = new WaveAuth()
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

    connectedCallback() {
        this.$wave = this.shadowRoot.querySelector("ui-wave")
        this.$requestbtn = this.shadowRoot.querySelector("#request-btn")
        this.$stopbtn = this.shadowRoot.querySelector("#stop-btn")
        this.$epub = this.shadowRoot.querySelector("#epub")
        this.$msg = this.shadowRoot.querySelector("#msg")
        this.$requestbtn.addEventListener("click", this.onrequestbtn)
        this.$stopbtn.addEventListener("click", this.onstopbtn)
        this.shadowRoot.querySelector("#passkey").addEventListener("click", this.passkey)
        this.shadowRoot.querySelector("#wave").addEventListener("click", this.wave)
        this.subscriptions.push(
            () => this.$requestbtn.removeEventListener("click", this.onrequestbtn),
            () => this.$stopbtn.removeEventListener("click", this.onstopbtn),
            () => this.shadowRoot.querySelector("#passkey").removeEventListener("click", this.passkey),
            () => this.shadowRoot.querySelector("#wave").removeEventListener("click", this.wave),
            this.$wave.events.on("message", this.onwave),
            this.states.on("method", this.render)
        )
        this.setstate("neutral")
        this.initpair()
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
        this.stop()
    }

    async initpair() {
        const { sea } = globalThis
        if (!sea?.pair) return
        const pair = await sea.pair()
        this.waveauth.session = pair
        if (this.$epub) {
            const epub = pair.epub
            this.$epub.textContent = !epub || epub.length <= 12 ? epub || "" : `${epub.slice(0, 5)}...${epub.slice(-5)}`
        }
    }

    setstate(state) {
        this.state = state
        this.$requestbtn.hidden = state !== "neutral"
        this.$stopbtn.hidden = state === "neutral"
        if (this.$msg) this.$msg.textContent = ""
    }

    done(response) {
        this.events.emit("done", { response }, { bubbles: true, composed: true })
    }

    async onwave(event) {
        const { parsed } = event?.detail || {}
        const result = await this.waveauth.handle(parsed)
        if (!result) return
        if (result.type === "deny") {
            if (this.$msg) this.$msg.textContent = "Access denied"
            return
        }
        if (result.type === "grant") {
            this.stop()
            wave({ seed: result.seed })
                .then((response) => this.done(response))
                .catch((error) => console.log(error))
        }
    }

    onrequestbtn() {
        this.setstate("listening")
        this.waveauth.request(this.$wave).catch((error) => {
            this.setstate("neutral")
            console.error(error)
        })
    }

    onstopbtn() {
        this.stop()
    }

    passkey() {
        this.states.set({ method: "passkey" })
        signinWithPasskey().then((response) => this.done(response))
    }

    wave() {
        this.states.set({ method: "wave" })
    }

    stop() {
        this.waveauth.reset()
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
