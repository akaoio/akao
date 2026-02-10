import template from "./template.js"
import { render } from "/core/UI.js"
import { Context } from "/core/Context.js"
import { notify, randomKey } from "/core/Utils.js"
import { Access } from "/core/Access.js"
import { Elements } from "/core/Stores.js"
import States from "/core/States.js"

export class ADDRESSES extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.states = new States({ addresses: {} })
        this.add = this.add.bind(this)
        this.save = this.save.bind(this)
        this.close = this.close.bind(this)
        this.reset = this.reset.bind(this)
        this.render = this.render.bind(this)
        this.subscriptions = []
    }

    connectedCallback() {
        this.form = this.shadowRoot.querySelector("#address-form")
        this.form.querySelectorAll("input[type='text'], input[type='email'], input[type='tel']").forEach((input) => this.subscriptions.push(Context.on(["dictionary", input.name], [input, "placeholder"])))
        this.shadowRoot.querySelector("#add").addEventListener("click", this.add)
        this.shadowRoot.querySelector("#save").addEventListener("click", this.save)
        this.shadowRoot.querySelector("#close").addEventListener("click", this.close)
        this.shadowRoot.querySelector("#reset").addEventListener("click", this.reset)
        
        this.subscriptions.push(
            () => this.shadowRoot.querySelector("#add").removeEventListener("click", this.add),
            () => this.shadowRoot.querySelector("#save").removeEventListener("click", this.save),
            () => this.shadowRoot.querySelector("#close").removeEventListener("click", this.close),
            () => this.shadowRoot.querySelector("#reset").removeEventListener("click", this.reset),
            this.states.on("addresses", this.render),
            Access.on("authenticated", () => {
                const { gun, sea } = globalThis
                const pair = Access.get("pair")
                if (pair) {
                    this.scope = this.scope || gun.get(`~${pair.pub}`).get("addresses").map()
                    this.scope?.off?.()
                    this.scope.on(async (data, key) => {
                        const addresses = { ...this.states.get("addresses"), [key]: await sea.decrypt(data, pair) }
                        this.states.set({ addresses })
                    })
                }
            })
        )
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
        this?.scope?.off?.()
    }

    add() {
        this.form.style.display = "flex"
        this.form.reset()
    }

    async save() {
        const check = Elements.Access?.checkpoint()
        if (!check) return
        const geo = this.shadowRoot.querySelector("ui-geo")
        // If there is an unseleted select in geo, do not proceed
        const selects = Array.from(geo.shadowRoot.querySelectorAll("ui-select"))
        if (!selects.at(-1)?.states?.get("selected") || !this.form.checkValidity()) {
            this.form.reportValidity()
            return notify({ content: Context.get(["dictionary", "missingRequiredFields"]) })
        }
        const data = { ...Object.fromEntries(new FormData(this.form)), geo: geo.states.get("id") }
        data.id = data?.id || randomKey()
        const { gun, sea } = globalThis
        const pair = Access.get("pair")
        if (!pair) return
        const encrypted = await sea.encrypt(data, pair)
        gun.get(`~${pair.pub}`).get("addresses").get(data.id).put(encrypted, null, { opt: { authenticator: pair } })
        this.close()
    }

    close() {
        this.form.style.display = "none"
        this.reset()
    }

    reset() {
        this.form.reset()
        this.form.querySelector("ui-geo").reset()
    }

    render() {
        // To be implemented: render the list of addresses
        console.log("Render addresses list", this.states.get("addresses"))

    }
}

customElements.define("ui-addresses", ADDRESSES)