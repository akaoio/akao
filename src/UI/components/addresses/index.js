import template from "./template.js"
import { render } from "/core/UI.js"
import { Context } from "/core/Context.js"
import { notify } from "/core/Utils.js"
import { Access } from "/core/Access.js"

export class ADDRESSES extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.add = this.add.bind(this)
        this.save = this.save.bind(this)
        this.close = this.close.bind(this)
        this.reset = this.reset.bind(this)
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
            () => this.shadowRoot.querySelector("#reset").removeEventListener("click", this.reset)
        )
    }

    add() {
        this.form.style.display = "flex"
        this.form.reset()
    }

    save() {
        const geo = this.shadowRoot.querySelector("ui-geo")
        // If there is an unseleted select in geo, do not proceed
        const selects = Array.from(geo.shadowRoot.querySelectorAll("ui-select"))
        if (!selects.at(-1)?.states?.get("selected") || !this.form.checkValidity()) {
            this.form.reportValidity()
            return notify({ content: Context.get(["dictionary", "missingRequiredFields"]) })
        }
        
        const data = {
            ...Object.fromEntries(new FormData(this.form)),
            geo: geo.states.get("id")
        }

        console.log("ACCESS", Access, data)
        // Put to GDB
        
        // this.close()
    }

    close() {
        this.form.style.display = "none"
        this.form.reset()
    }

    reset() {
        this.form.reset()
        this.form.querySelector("ui-geo").reset()
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }
}

customElements.define("ui-addresses", ADDRESSES)