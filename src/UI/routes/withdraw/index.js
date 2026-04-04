import template from "./template.js"
import { render } from "/core/UI.js"
import { Context } from "/core/Context.js"
import { notify } from "/core/Utils.js"
import { Elements, Wallets } from "/core/Stores.js"
import logic from "./logic.js"

export class WITHDRAW extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
        this.submit = this.submit.bind(this)
        this.estimateGas = this.estimateGas.bind(this)
    }

    connectedCallback() {
        this.$wallets = this.shadowRoot.querySelector("ui-wallets")
        this.$form = this.shadowRoot.querySelector("#form")
        this.$gas = this.shadowRoot.querySelector("#gas")
        const $submit = this.shadowRoot.querySelector("#submit")

        this.$form.querySelectorAll("input[type='text'], input[type='number']").forEach((input) => {
            this.subscriptions.push(Context.on(["dictionary", input.name], [input, "placeholder"]))
            input.addEventListener("input", this.estimateGas)
            this.subscriptions.push(() => input.removeEventListener("input", this.estimateGas))
        })

        $submit.addEventListener("click", this.submit)

        this.subscriptions.push(
            this.$wallets.states.on("address", ({ value }) => {
                this.$form.querySelectorAll("input").forEach((el) => (el.disabled = !value))
                $submit.toggleAttribute("disabled", !value)
                if (!value) {
                    this.$form.reset()
                    this.$gas.textContent = ""
                } else 
                    this.estimateGas()
                
            }, true),
            () => $submit.removeEventListener("click", this.submit)
        )

        Elements.Access?.checkpoint()
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }

    estimateGas() {
        clearTimeout(this.$gaspend)
        this.$gaspend = setTimeout(async () => {
            const chain = this.$wallets.states.get("chain")
            const fee = await logic.gas({
                wallet: Wallets[chain],
                name: this.$wallets.states.get("currency"),
                to: this.$form.querySelector("input[name='address']").value,
                amount: this.$form.querySelector("input[name='amount']").value,
            })
            if (!fee) { this.$gas.textContent = ""; return }
            const label = Context.get(["dictionary", "gasFee"])
            this.$gas.textContent = `${label}: ${fee.amount} ${fee.symbol}`
        }, 500)
    }

    async submit() {
        const data = Object.fromEntries(new FormData(this.$form))
        const chain = this.$wallets.states.get("chain")
        const name = this.$wallets.states.get("currency")

        if (!data.amount || !data.address || !chain || !name)
            return notify({ content: Context.get(["dictionary", "missingRequiredFields"]), autoClose: true })

        this.$form.querySelectorAll("input").forEach((el) => (el.disabled = true))
        this.shadowRoot.querySelector("#submit").toggleAttribute("disabled", true)

        const result = await logic.send({ wallet: Wallets[chain], name, to: data.address, amount: data.amount })

        this.$form.querySelectorAll("input").forEach((el) => (el.disabled = false))
        this.shadowRoot.querySelector("#submit").toggleAttribute("disabled", false)

        if (result.success) {
            notify({ content: Context.get(["dictionary", "transactionSent"]), autoClose: true })
            this.$form.reset()
        } else
            notify({ content: Context.get(["dictionary", result.error || "transactionError"]), autoClose: true })
    }
}

customElements.define("route-withdraw", WITHDRAW)

export default WITHDRAW
