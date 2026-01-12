import template from "./template.js"
import { Context } from "/core/Context.js"
import DB from "/core/DB.js"
import Router from "/core/Router.js"
import { html, render } from "/core/UI.js"

export class ITEM extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
        this.render = this.render.bind(this)
        this.increase = this.increase.bind(this)
        this.decrease = this.decrease.bind(this)
        this.add = this.add.bind(this)
    }

    connectedCallback() {
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
        const data = Object.fromEntries(new FormData(form))
        form.reset()
        console.log("add to cart", data)
    }

    async render() {
        const key = Context.get("params").item || globalThis.history.state?.params?.item
        const meta = await DB.get(["statics", "items", key, "meta.json"])
        const data = await DB.get(["statics", "items", key, `${Context.get("locale").code}.json`])
        if (!data) return
        Router.setHead({
            title: data.name,
            description: data.description || ""
        })
        Context.set({ item: { ...meta, ...data } })

        if (this.shadowRoot.querySelector("#attributes").children.length == 0) {
            const attributes = meta.attributes.map(
                (attr) => html`
                    <section>
                        <header><ui-context data-key="dictionary.${attr.name}" /></header>
                        ${attr.values.map(
                            (value) => html`
                                <input id="${value}" type="radio" name="${attr.name}" value="${value}" />
                                <label for="${value}"><ui-context data-key="dictionary.${value}" /></label>
                            `
                        )}
                    </section>
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
