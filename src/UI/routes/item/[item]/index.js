import template from "./template.js"
import { Context } from "/core/Context.js"
import DB from "/core/DB.js"
import Router from "/core/Router.js"
import { html, render } from "/core/UI.js"
import { sha256, notify } from "/core/Utils.js"
import { States } from "/core/States.js"
import { Indexes } from "/core/Stores.js"

export class ITEM extends HTMLElement {
    constructor() {
        super()
        this.states = new States({ item: Context.get("params").item || globalThis.history.state?.params?.item })
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
        this.render = this.render.bind(this)
        this.increase = this.increase.bind(this)
        this.decrease = this.decrease.bind(this)
        this.add = this.add.bind(this)
    }

    async connectedCallback() {
        const meta = await DB.get(["statics", "items", this.states.get("item"), "meta.json"])
        this.states.set({ meta })
        this.subscriptions.push(Context.on("locale", this.render))
        this.render()
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
        Context.del("item")
    }

    decrease() {
        this.shadowRoot.querySelector("#quantity input").stepDown()
    }

    increase() {
        this.shadowRoot.querySelector("#quantity input").stepUp()
    }

    add() {
        const form = this.shadowRoot.querySelector("#item")
        if (!form.checkValidity()) {
            form.reportValidity()
            return
        }
        const data = Object.fromEntries(new FormData(form))
        form.reset()
        let { item, sku, quantity, ...rest } = data
        notify({
            content: html`<ui-context data-key="item.name" /> x ${quantity}`,
            autoClose: true
        })
        const key = sha256(`item:${item}.sku:${sku}.${Object.entries(rest).sort().map(([k, v]) => `${k}:${v}`).join(".")}`)
        quantity = Number(quantity) || 1
        const cart = Context.get("cart") || {}
        if (cart[key]) {
            cart[key].quantity += quantity
            cart[key].timestamp = Date.now()
        }
        else cart[key] = {item, sku, quantity, ...rest, timestamp: Date.now()}
        Context.set({ cart })
        Indexes.Cart.get("cart").put(cart)
    }

    async render() {
        const item = this.states.get("item")
        const meta = this.states.get("meta")
        const data = await DB.get(["statics", "items", item, `${Context.get("locale").code}.json`])
        if (!data) return
        this.states.set({ data })
        Router.setHead({
            title: data.name,
            description: data.description || ""
        })
        Context.set({ item: { ...meta, ...data } })
        this.shadowRoot.querySelector("input[name=item]").value = item
        this.shadowRoot.querySelector("input[name=sku]").value = meta.sku
        if (this.shadowRoot.querySelector("#attributes").children.length == 0) {
            const attributes = meta.attributes.map(
                (attr) => html`
                    <fieldset>
                        <legend><ui-context data-key="dictionary.${attr.name}" /></legend>
                        ${attr.values.map(
                            (value) => html`
                                <input id="${value}" type="radio" name="${attr.name}" value="${value}" required />
                                <label for="${value}"><ui-context data-key="dictionary.${value}" /></label>
                            `
                        )}
                    </fieldset>
                `
            )
            render(attributes, this.shadowRoot.querySelector("#attributes"))
        }
        this.shadowRoot.querySelector("#decrease").addEventListener("click", this.decrease)
        this.shadowRoot.querySelector("#increase").addEventListener("click", this.increase)
        this.shadowRoot.querySelector("#add").addEventListener("click", this.add)
        this.subscriptions.push(
            () => this.shadowRoot.querySelector("#decrease").removeEventListener("click", this.decrease),
            () => this.shadowRoot.querySelector("#increase").removeEventListener("click", this.increase),
            () => this.shadowRoot.querySelector("#add").removeEventListener("click", this.add)
        )
    }
}

customElements.define("route-item", ITEM)

export default ITEM
