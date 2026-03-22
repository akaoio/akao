import { render } from "/core/UI.js"
import template from "./template.js"
import { threads } from "/core/Threads.js"

export class WAVE extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.listen = this.listen.bind(this)
        this.send = this.send.bind(this)
        this.play = this.play.bind(this)
    }

    async listen() {
        console.log("Listening for waves...")
        try {
            if (!navigator?.mediaDevices?.getUserMedia) {
                throw new Error("Microphone is not supported in this browser")
            }
            const stream = await navigator.mediaDevices.getUserMedia({ audio: true })
            console.log("Microphone access granted")
            return stream
        } catch (error) {
            console.error("Microphone access denied", error)
            return null
        }
    }

    send() {
        console.log("Sending a wave...")
    }

    play() {
        console.log("Playing a wave...")
    }
}

customElements.define("ui-wave", WAVE)