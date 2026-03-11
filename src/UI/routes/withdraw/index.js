import template from "./template.js"
import { render } from "/core/UI.js"
import { Context } from "/core/Context.js"
import { notify } from "/core/Utils.js"
import { Elements, Wallets } from "/core/Stores.js"

export class WITHDRAW extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
        this.submit = this.submit.bind(this)
    }

    connectedCallback() {
        this.$wallets = this.shadowRoot.querySelector("ui-wallets")
        this.$form = this.shadowRoot.querySelector("#form")
        const $submit = this.shadowRoot.querySelector("#submit")

        this.$form.querySelectorAll("input[type='text'], input[type='number']").forEach((input) =>
            this.subscriptions.push(Context.on(["dictionary", input.name], [input, "placeholder"]))
        )

        $submit.addEventListener("click", this.submit)

        this.subscriptions.push(
            this.$wallets.states.on("address", ({ value }) => {
                this.$form.querySelectorAll("input").forEach((el) => (el.disabled = !value))
                $submit.toggleAttribute("disabled", !value)
                if (!value) this.$form.reset()
            }, true),
            () => $submit.removeEventListener("click", this.submit)
        )

        Elements.Access?.checkpoint()
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }

    async submit() {
        const data = Object.fromEntries(new FormData(this.$form))
        const chain = this.$wallets.states.get("chain")
        const currencyName = this.$wallets.states.get("currency")

        if (!data.amount || !data.address || !chain || !currencyName) {
            return notify({ content: Context.get(["dictionary", "missingRequiredFields"]), autoClose: true })
        }

        const wallet = Wallets[chain]
        const currency = Object.values(wallet.chain.currencies).find((c) => c.name === currencyName)
        if (!currency) return

        this.$form.querySelectorAll("input").forEach((el) => (el.disabled = true))
        this.shadowRoot.querySelector("#submit").toggleAttribute("disabled", true)

        const tx = await wallet.send({ to: data.address, amount: Number(data.amount), currency })

        this.$form.querySelectorAll("input").forEach((el) => (el.disabled = false))
        this.shadowRoot.querySelector("#submit").toggleAttribute("disabled", false)

        if (tx?.error) {
            notify({ content: Context.get(["dictionary", "transactionError"]), autoClose: true })
        } else {
            notify({ content: Context.get(["dictionary", "transactionSent"]), autoClose: true })
            this.$form.reset()
        }
    }
}

customElements.define("route-withdraw", WITHDRAW)

export default WITHDRAW
