import template from "./template.js"
import { Context } from "/core/Context.js"
import DB from "/core/DB.js"
import Router from "/core/Router.js"
import { html, render } from "/core/UI.js"
import { notify } from "/core/Utils.js"
import { States } from "/core/States.js"
import Cart from "/core/Cart.js"

export class ITEM extends HTMLElement {
    constructor() {
        super()
        this.states = new States({ id: Context.get("params").item || globalThis.history.state?.params?.item })
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
        this.render = this.render.bind(this)
        this.increase = this.increase.bind(this)
        this.decrease = this.decrease.bind(this)
        this.add = this.add.bind(this)
    }

    async connectedCallback() {
        const meta = await DB.get(["statics", "items", this.states.get("id"), "meta.json"])
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

    async add() {
        const form = this.shadowRoot.querySelector("#item")
        if (!form.checkValidity()) {
            form.reportValidity()
            return
        }
        const item = Object.fromEntries(new FormData(form))
        form.reset()
        const result = await Cart.add(item)
        if (result) notify({ content: `${Context.get(["dictionary", "itemAddedToCart"])}: ${Context.get(["item", "name"])} x ${item?.quantity}` })
    }

    async render() {
        const id = this.states.get("id")
        const meta = this.states.get("meta")
        const data = await DB.get(["statics", "items", id, `${Context.get("locale").code}.json`])
        if (!data) return
        this.states.set({ data })
        Router.setHead({
            title: data.name,
            description: data.description || ""
        })
        Context.set({ item: { ...meta, ...data } })
        this.shadowRoot.querySelector("input[name=id]").value = id
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
