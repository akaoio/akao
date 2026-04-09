import template from "./template.js"
import { Context } from "/core/Context.js"
import { html, render } from "/core/UI.js"
import Component from "/core/UI/Component.js"
import Cart from "/core/Cart.js"
import logic from "./logic.js"

export class CART extends Component {
    static module = import.meta.url
    constructor() {
        super()
        this.template = template // Store for HMR
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.render = this.render.bind(this)
    }

    onconnect() {
        const button = this.shadowRoot.querySelector("ui-icon")
        this.modal = this.shadowRoot.querySelector("ui-modal")
        this.listen(button, "click", this.modal.toggleModal.bind(this.modal))
        this.sub(Cart.states.on("list", this.render), Context.on("locale", this.render))
        this.listen(this.modal, "click", (e) => {
            const icon = e.composedPath().find(el => el.tagName === "UI-ICON")
            if (!icon) return
            const itemDiv = icon.closest?.(".item")
            if (!itemDiv) return
            const index = [...itemDiv.parentNode.children].indexOf(itemDiv)
            const item = Cart.states.get("list")[index]
            if (!item) return
            if (icon.dataset.icon === "dash-lg") Cart.decrease(item.key, 1)
            else if (icon.dataset.icon === "plus-lg") Cart.increase(item.key, 1)
            else if (icon.dataset.icon === "x-lg") Cart.remove(item.key)
        })
        this.render()
    }

    async render() {
        const list = []
        for (const item of Cart.states.get("list")) {
            const $item = await logic.item(item.id, Context.get("locale").code)
            list.push(html`
                <div class="item">
                    <div><a is="ui-a" data-to="/item/${item.id}">${$item.name}</a></div>
                    <div><ui-fiat data-amount="${item.total}" /></div>
                    <div class="actions">
                        <ui-icon data-size="sm" data-icon="dash-lg" />
                        <span>${item.quantity}</span>
                        <ui-icon data-size="sm" data-icon="plus-lg" />
                        <ui-icon data-size="sm" data-icon="x-lg" />
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
