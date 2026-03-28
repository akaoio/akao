import { render } from "/core/UI.js"
import template from "./template.js"
import Events from "/core/Events.js"
import { wave } from "/core/Access.js"
import WaveAuth from "./wave.js"
import signinWithPasskey from "./passkey.js"

export class AUTHENTICATE extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.events = new Events(this)
        this.subscriptions = []
        this.waveauth = new WaveAuth()
        this.onwave = this.onwave.bind(this)
        this.onwavebtn = this.onwavebtn.bind(this)
        this.onpasskeybtn = this.onpasskeybtn.bind(this)
        this.stop = this.stop.bind(this)
        this.startsignin = this.startsignin.bind(this)
        this.startshare = this.startshare.bind(this)
    }

    connectedCallback() {
        this.$wave = this.shadowRoot.querySelector("#wave")
        this.$vis = this.shadowRoot.querySelector("#vis")
        this.$wavesection = this.shadowRoot.querySelector(".wave-section")
        this.$wavebtn = this.shadowRoot.querySelector("#wave-btn")
        this.$passkeybtn = this.shadowRoot.querySelector("#passkey-btn")
        this.$wavebtn.addEventListener("click", this.onwavebtn)
        this.$passkeybtn.addEventListener("click", this.onpasskeybtn)
        this.subscriptions.push(
            () => this.$wavebtn.removeEventListener("click", this.onwavebtn),
            () => this.$passkeybtn.removeEventListener("click", this.onpasskeybtn),
            this.$wave.events.on("message", this.onwave),
            this.$wave.events.on("stream", (e) => this.$vis?.stream(e.detail?.stream ?? null))
        )
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
        this.stop()
    }

    showwave(visible) {
        this.$wavesection?.classList?.toggle("active", visible)
    }

    done(response) {
        this.events.emit("done", { response }, { bubbles: true, composed: true })
    }

    onwave(event) {
        this.waveauth.onmessage(event, this.$wave, ({ seed, from, channel }) => {
            wave({ seed, from, channel }).then((response) => this.done(response))
        })
    }

    onwavebtn() {
        this.showwave(true)
        this.startsignin().catch((error) => console.error(error))
    }

    onpasskeybtn() {
        this.showwave(false)
        signinWithPasskey().then((response) => this.done(response))
    }

    startsignin() {
        return this.waveauth.startsignin(this.$wave)
    }

    startshare(builder) {
        this.showwave(true)
        return this.waveauth.startshare(this.$wave, builder)
    }

    stop() {
        this.waveauth.stop()
        this.$wave?.stop?.()
        this.showwave(false)
    }
}

customElements.define("ui-authenticate", AUTHENTICATE)
export default AUTHENTICATE
