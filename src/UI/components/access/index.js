import { Elements } from "/core/Stores.js"
import { Context } from "/core/Context.js"
import { Access } from "/core/Access.js"
import template from "./template.js"
import { render } from "/core/UI.js"
import Component from "/core/UI/Component.js"
import logic from "./logic.js"

export class ACCESS extends Component {
    static module = import.meta.url
    constructor() {
        super()
        this.template = template // Store for HMR
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.next = this.next.bind(this)
        this.checkpoint = this.checkpoint.bind(this)
        this.show = this.show.bind(this)
        this.signupScreen = this.signupScreen.bind(this)
        this.signinScreen = this.signinScreen.bind(this)
        this.unauthenticated = this.unauthenticated.bind(this)
        this.signup = this.signup.bind(this)
        this.ondone = this.ondone.bind(this)
        this.openWaveSignin = this.openWaveSignin.bind(this)
        this.onmodalclose = this.onmodalclose.bind(this)
    }

    onconnect() {
        // Assign to Elements only after component is fully connected and modal is initialized
        Elements.Access = this
        this.modal = this.shadowRoot.querySelector("ui-modal")
        this.$dialog = this.modal.shadowRoot?.querySelector("dialog")
        this.form = this.shadowRoot.querySelector("#signup-form")
        this.auth = this.shadowRoot.querySelector("#auth")
        this.$back = this.shadowRoot.querySelector("#back")
        this.listen(this.$back, "click", this.unauthenticated)
        this.listen(this.shadowRoot.querySelector("#signup"), "click", this.signupScreen)
        this.listen(this.shadowRoot.querySelector("#confirm"), "click", this.signup)
        this.listen(this.shadowRoot.querySelector("#signin"), "click", this.signinScreen)
        this.listen(this.$dialog, "close", this.onmodalclose)
        this.sub(
            this.auth?.events?.on?.("done", this.ondone)
        )
        this.form.querySelectorAll("input[type='text']").forEach((input) => this.sub(Context.on(["dictionary", input.name], [input, "placeholder"])))
    }

    ondisconnect() {
        if (Elements.Access === this) Elements.Access = null
    }

    onmodalclose() {
        this.auth?.stop?.()
        this.auth?.reset?.()
        this.unauthenticated()
    }

    next(response) {
        if (response.error) return console.error(response)
        this.form.reset()
        this.modal.close()
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
        this.auth?.stop?.()
        this.auth?.reset?.()
        this.show("unauthenticated-screen")
        this.$back.hidden = true
    }

    signup() {
        const data = Object.fromEntries(new FormData(this.form))
        logic.signup(data).then(this.next)
    }

    openWaveSignin() {
        this.modal.showModal()
        this.show("signin-screen")
    }

    sign(data, callback) {
        this.modal.showModal()
        this.show("sign-screen")
        this.shadowRoot.querySelector("#sign").addEventListener("click", () =>
            logic.sign(data).then((data) => {
                if (typeof callback === "function") callback(data)
                this.modal.close()
            })
        )
    }
}

customElements.define("ui-access", ACCESS)

export default ACCESS
