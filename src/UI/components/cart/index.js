import template from "./template.js"
import { Context } from "/core/Context.js"
import { html, render } from "/core/UI.js"
import BaseElement from "/UI/BaseElement.js"
import Cart from "/core/Cart.js"
import logic from "./logic.js"

export class CART extends BaseElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.render = this.render.bind(this)
    }

    onConnect() {
        const button = this.shadowRoot.querySelector("ui-icon")
        this.modal = this.shadowRoot.querySelector("ui-modal")
        this.listen(button, "click", this.modal.toggleModal.bind(this.modal))
        this.subscribe(Cart.states.on("list", this.render), Context.on("locale", this.render))
        this.render()
    }

    async render() {
        const list = []
        for (const item of Cart.states.get("list")) {
            const $item = await logic.item(item.id, Context.get("locale").code)
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
