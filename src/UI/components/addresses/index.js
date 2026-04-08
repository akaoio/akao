import template from "./template.js"
import { html, render } from "/core/UI.js"
import { Context } from "/core/Context.js"
import { notify, randomKey } from "/core/Utils.js"
import { Access } from "/core/Access.js"
import { Elements } from "/core/Stores.js"
import States from "/core/States.js"
import BaseElement from "/UI/BaseElement.js"
import "/UI/components/icon/index.js"
import logic from "./logic.js"

export class ADDRESSES extends BaseElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.states = new States({ addresses: {}, current: null })
        this.add = this.add.bind(this)
        this.save = this.save.bind(this)
        this.close = this.close.bind(this)
        this.reset = this.reset.bind(this)
        this.edit = this.edit.bind(this)
        this.delete = this.delete.bind(this)
        this.confirm = this.confirm.bind(this)
        this.cancel = this.cancel.bind(this)
        this.render = this.render.bind(this)
    }

    async onConnect() {
        this.countries = await logic.countries()
        this.modal = this.shadowRoot.querySelector("ui-modal#deletion")
        this.form = this.shadowRoot.querySelector("#address-form")
        this.form.querySelectorAll("input[type='text'], input[type='email'], input[type='tel']").forEach((input) => this.subscribe(Context.on(["dictionary", input.name], [input, "placeholder"])))
        this.listen(this.shadowRoot.querySelector("#cancel"), "click", this.cancel)
        this.listen(this.shadowRoot.querySelector("#add"), "click", this.add)
        this.listen(this.shadowRoot.querySelector("#save"), "click", this.save)
        this.listen(this.shadowRoot.querySelector("#close"), "click", this.close)
        this.listen(this.shadowRoot.querySelector("#reset"), "click", this.reset)
        this.listen(this.shadowRoot.querySelector("#confirm"), "click", this.confirm)
        const $delete = () => {
            const id = this.states.get("current")
            if (!id) return
            this.delete(id)
        }
        this.listen(this.shadowRoot.querySelector("#delete"), "click", $delete)
        this.subscribe(
            this.states.on("addresses", this.render),
            Access.on("authenticated", () => {
                if (logic.pair()) {
                    this.scope?.off?.()
                    this.scope = logic.watch((key, address) => {
                        const addresses = { ...this.states.get("addresses") }
                        addresses[key] = address
                        this.states.set({ addresses })
                    })
                    this.subscribe(() => this.scope?.off?.())
                }
            })
        )
    }

    onDisconnect() {
        this?.scope?.off?.()
    }

    add() {
        this.shadowRoot.querySelector("#addresses").style.display = "none"
        this.form.style.display = "flex"
        this.form.reset()
    }

    async edit(id) {
        id = id || this.states.get("current")
        if (this.states.get("current") !== id) this.states.set({ current: id })
        this.shadowRoot.querySelector("#addresses").style.display = "none"
        this.form.style.display = "flex"
        if (!logic.pair()) return
        const data = await logic.read(id)
        this.form.querySelectorAll("input").forEach((input) => {
            if (input.name in data) 
                if (input.type === "checkbox") input.checked = data[input.name]
                else if (input.type === "radio" && input.value === data[input.name]) input.checked = true
                else input.value = data[input.name]
            
        })
        const { billing, shipping, encrypted } = await logic.defaults(id)
        if (billing === encrypted) this.form.querySelector("input[name='billing']").checked = true
        if (shipping === encrypted) this.form.querySelector("input[name='shipping']").checked = true
        const geo = this.form.querySelector("ui-geo")
        geo.states.set({ id: data.geo || null })
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
        const address = { ...Object.fromEntries(new FormData(this.form)), geo: geo.states.get("id") }
        address.id = address?.id || randomKey()

        const { billing, shipping } = address
        delete address.billing
        delete address.shipping
        
        if (!logic.pair()) return
        await logic.write(address, { billing, shipping })

        const element = this.shadowRoot.querySelector(`#addresses #${address.id}`)
        if (element) {
            const area = await logic.area(address.geo, this.countries)
            element.querySelector(".name").textContent = `${address.firstName} ${address.lastName}`
            element.querySelector(".address").innerHTML = `${address.addressLine1}<br/>${address.addressLine2}`
            element.querySelector(".postalCode").textContent = address.postalCode
            element.querySelector(".area").textContent = area
        }
        this.close()
    }

    delete(id) {
        id = id || this.states.get("current")
        if (this.states.get("current") !== id) this.states.set({ current: id })
        this.modal.showModal()
    }

    confirm() {
        const id = this.states.get("current")
        if (id) {
            logic.remove(id)
        }
        this.states.set({ current: null })
        const addresses = { ...this.states.get("addresses") }
        delete addresses[id]
        this.states.set({ addresses })
        this.shadowRoot.querySelector(`#addresses #${id}`)?.remove()
        this.modal.close()
        this.close()
    }

    cancel() {
        this.states.set({ current: null })
        this.modal.close()
    }

    close() {
        this.form.style.display = "none"
        this.shadowRoot.querySelector("#addresses").style.display = "flex"
        this.reset()
    }

    reset() {
        this.form.reset()
        this.form.querySelector("ui-geo").reset()
        this.states.set({ current: null })
    }

    async render() {
        const container = this.shadowRoot.querySelector("#addresses")
        const addresses = Object.entries(this.states.get("addresses"))
        for (const [id, address] of addresses) {
            const area = await logic.area(address.geo, this.countries)
            let element = this.shadowRoot.querySelector(`#${id}`)
            if (!element) {
                element = html`<div id="${id}">
                    <main>
                        <span class="name">${address.firstName} ${address.lastName}</span>
                        <span class="address">${address.addressLine1}<br/>${address.addressLine2}</span>
                        <span class="postalCode">${address.postalCode}</span>
                        <span class="area">${area}</span>
                    </main>
                    <nav>
                        <ui-icon data-icon="pen" data-size="md" class="edit" ${({element}) => {
                            const edit = () => this.edit(id)
                            element.addEventListener("click", edit)
                            this.subscriptions.push(() => element.removeEventListener("click", edit))
                        }} />
                        <ui-icon data-icon="trash3" data-size="md" class="delete" ${({element}) => {
                            const $delete = () => this.delete(id)
                            element.addEventListener("click", $delete)
                            this.subscriptions.push(() => element.removeEventListener("click", $delete))
                        }} />
                    </nav>
                </div>`
                render(element, container, { append: true })
            } else {
                element.querySelector(".name").textContent = `${address.firstName} ${address.lastName}`
                element.querySelector(".address").innerHTML = `${address.addressLine1}<br/>${address.addressLine2}`
                element.querySelector(".postalCode").textContent = address.postalCode
                element.querySelector(".area").textContent = area
            }
        }
    }
}

customElements.define("ui-addresses", ADDRESSES)