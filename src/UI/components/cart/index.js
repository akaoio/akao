import template from "./template.js"
import { Context } from "/core/Context.js"
import { html, render } from "/core/UI.js"
import Cart from "/core/Cart.js"
import DB from "/core/DB.js"

export class CART extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
        this.render = this.render.bind(this)
    }

    connectedCallback() {
        const button = this.shadowRoot.querySelector("ui-icon")
        this.modal = this.shadowRoot.querySelector("ui-modal")
        button.addEventListener("click", this.modal.toggleModal)
        this.subscriptions.push(() => button.removeEventListener("click", this.modal.toggleModal), Cart.states.on("list", this.render), Context.on("locale", this.render))
        this.render()
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }

    async render() {
        const list = []
        for (const item of Cart.states.get("list")) {
            const $item = await DB.get(["statics", "items", item.id, `${Context.get("locale").code}.json`])
            const increase = () => Cart.increase(item.key, 1)
            const decrease = () => Cart.decrease(item.key, 1)
            const remove = () => Cart.remove(item.key)
            list.push(html`
                <div class="item">
                    <div><a is="ui-a" data-to="/item/${item.id}">${$item.name}</a></div>
                    <div><ui-fiat data-amount="${item.total}" /></div>
                    <div class="actions">
                        <ui-icon
                            data-size="sm"
                            data-icon="dash-lg"
                            ${({ element }) => {
                                element.addEventListener("click", decrease)
                                this.subscriptions.push(() => element.removeEventListener("click", decrease))
                            }} />
                        <span>${item.quantity}</span>
                        <ui-icon
                            data-size="sm"
                            data-icon="plus-lg"
                            ${({ element }) => {
                                element.addEventListener("click", increase)
                                this.subscriptions.push(() => element.removeEventListener("click", increase))
                            }} />
                        <ui-icon
                            data-size="sm"
                            data-icon="x-lg"
                            ${({ element }) => {
                                element.addEventListener("click", remove)
                                this.subscriptions.push(() => element.removeEventListener("click", remove))
                            }} />
                    </div>
                </div>
            `)
        }
        render(list, this.modal.querySelector("#items"))
        this.modal.querySelector("#total ui-fiat").dataset.amount = Cart.states.get("total")
    }
}

customElements.define("ui-cart", CART)

export default CART
