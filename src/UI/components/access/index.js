import { Elements } from "/core/Stores.js"
import { Context } from "/core/Context.js"
import { Access, signup } from "/core/Access.js"
import template from "./template.js"
import { render } from "/core/UI.js"
import WebAuthn from "/core/WebAuthn.js"

export class ACCESS extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
        this.next = this.next.bind(this)
        this.checkpoint = this.checkpoint.bind(this)
        this.show = this.show.bind(this)
        this.signupScreen = this.signupScreen.bind(this)
        this.signinScreen = this.signinScreen.bind(this)
        this.unauthenticated = this.unauthenticated.bind(this)
        this.signup = this.signup.bind(this)
        this.ondone = this.ondone.bind(this)
        this.openWaveSignin = this.openWaveSignin.bind(this)
    }

    connectedCallback() {
        // Assign to Elements only after component is fully connected and modal is initialized
        Elements.Access = this
        this.modal = this.shadowRoot.querySelector("ui-modal")
        this.form = this.shadowRoot.querySelector("#signup-form")
        this.auth = this.shadowRoot.querySelector("#auth")
        this.$back = this.shadowRoot.querySelector("#back")
        this.$back.addEventListener("click", this.unauthenticated)
        this.shadowRoot.querySelector("#signup").addEventListener("click", this.signupScreen)
        this.shadowRoot.querySelector("#confirm").addEventListener("click", this.signup)
        this.shadowRoot.querySelector("#signin").addEventListener("click", this.signinScreen)
        this.subscriptions.push(
            () => this.$back.removeEventListener("click", this.unauthenticated),
            () => this.shadowRoot.querySelector("#signup").removeEventListener("click", this.signupScreen),
            () => this.shadowRoot.querySelector("#confirm").removeEventListener("click", this.signup),
            () => this.shadowRoot.querySelector("#signin").removeEventListener("click", this.signinScreen),
            this.auth?.events?.on?.("done", this.ondone)
        )
        this.form.querySelectorAll("input[type='text']").forEach((input) => this.subscriptions.push(Context.on(["dictionary", input.name], [input, "placeholder"])))
    }

    disconnectedCallback() {
        if (Elements.Access === this) Elements.Access = null
        this.subscriptions.forEach((off) => off())
    }

    next(response) {
        if (response.error) return console.error(response)
        this.form.reset()
        this.modal.close()
        this.auth?.stop?.()
    }

    ondone(event) {
        this.next(event?.detail?.response)
    }

    checkpoint() {
        if (Access.get("authenticated")) return true
        this.modal.showModal()
        if (!Access.get("authenticated")) this.show("unauthenticated-screen")
        return false
    }

    show(id) {
        this.shadowRoot.querySelectorAll("main").forEach((e) => e.classList.remove("active"))
        this.shadowRoot.getElementById(id).classList.add("active")
    }

    signupScreen() {
        this.form.reset()
        this.show("signup-screen")
        this.$back.hidden = false
    }

    signinScreen() {
        this.auth?.reset?.()
        this.show("signin-screen")
        this.$back.hidden = false
    }

    unauthenticated() {
        this.show("unauthenticated-screen")
        this.$back.hidden = true
    }

    signup() {
        const data = Object.fromEntries(new FormData(this.form))
        signup(data).then(this.next)
    }

    openWaveSignin() {
        this.modal.showModal()
        this.show("signin-screen")
    }

    sign(data, callback) {
        this.modal.showModal()
        this.show("sign-screen")
        this.shadowRoot.querySelector("#sign").addEventListener("click", () =>
            WebAuthn.sign({ data }).then((data) => {
                if (typeof callback === "function") callback(data)
                this.modal.close()
            })
        )
    }
}

customElements.define("ui-access", ACCESS)

export default ACCESS
