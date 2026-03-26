import template from "./template.js"
import { Context } from "/core/Context.js"
import DB from "/core/DB.js"
import Router from "/core/Router.js"
import { html, render } from "/core/UI.js"
import { notify } from "/core/Utils.js"
import { States } from "/core/States.js"
import Cart from "/core/Cart.js"

function camelToLabel(key) {
    return key.replace(/([A-Z])/g, " $1").replace(/^./, (c) => c.toUpperCase()).trim()
}

function resolveDisplayType(meta = {}) {
    return meta.type || meta.itemTypeName || meta.configs?.type || ""
}

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
            return notify({ content: Context.get(["dictionary", "missingRequiredFields"]) })
        }
        const item = Object.fromEntries(new FormData(form))
        form.reset()
        const result = await Cart.add(item)
        if (result) notify({ content: `${Context.get(["dictionary", "itemAddedToCart"])}: ${Context.get(["item", "name"])} x ${item?.quantity}` })
    }

    _renderGameData(meta) {
        const root = this.shadowRoot

        // Icon
        const icon = root.querySelector("#icon")
        if (meta.icon) {
            icon.src = meta.icon
            icon.alt = meta.name || ""
            icon.style.display = ""
        } else 
            icon.style.display = "none"
        

        // Rarity + type badges
        const rarityKey = (meta.rarity || "common").toLowerCase().replace(/\s+/g, "-")
        const rarityColor = `var(--rarity-${rarityKey}, var(--color-accent, #888))`
        this.style.setProperty("--item-rarity-color", rarityColor)

        const rarityBadge = root.querySelector("#rarity-badge")
        rarityBadge.textContent = meta.rarity || ""
        rarityBadge.style.background = rarityColor
        rarityBadge.style.display = meta.rarity ? "" : "none"

        const typeBadge = root.querySelector("#type-badge")
        const displayType = resolveDisplayType(meta)
        typeBadge.textContent = displayType
        typeBadge.style.display = displayType ? "" : "none"

        const subtypeBadge = root.querySelector("#subtype-badge")
        subtypeBadge.textContent = meta.subtype || ""
        subtypeBadge.style.display = meta.subtype ? "" : "none"

        // Breadcrumb
        const backLink = root.querySelector("#back-link")
        if (meta.game) {
            backLink.textContent = meta.game.replace(/-/g, " ").replace(/\b\w/g, (c) => c.toUpperCase())
            backLink.dataset.to = `/game/${meta.game}`
            root.querySelector("#breadcrumb").style.display = ""
        } else 
            root.querySelector("#breadcrumb").style.display = "none"
        

        // Flavor text
        const flavorEl = root.querySelector("#flavor-text")
        flavorEl.textContent = meta.flavor_text || ""
        flavorEl.style.display = meta.flavor_text ? "" : "none"

        // Stat block (rendered once — meta doesn't change on locale switch)
        const statBlock = root.querySelector("#stat-block")
        if (statBlock.children.length === 0 && meta.stat_block) {
            const rows = Object.entries(meta.stat_block).map(([key, val]) =>
                html`<div class="stat-row"><dt>${camelToLabel(key)}</dt><dd>${val}</dd></div>`
            )
            render(rows, statBlock)
            root.querySelector("#stats").style.display = ""
        } else if (!meta.stat_block) 
            root.querySelector("#stats").style.display = "none"
        

        // Loadout slots
        const slotsEl = root.querySelector("#loadout-slots")
        if (slotsEl.children.length === 0 && meta.loadout_slots?.length) {
            const chips = meta.loadout_slots.map((slot) =>
                html`<span class="slot-chip">${camelToLabel(slot)}</span>`
            )
            render(chips, slotsEl)
            root.querySelector("#slots").style.display = ""
        } else if (!meta.loadout_slots?.length) 
            root.querySelector("#slots").style.display = "none"
        
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

        const root = this.shadowRoot
        root.querySelector("#breadcrumb-name").textContent = data.name || ""
        root.querySelector("input[name=id]").value = id
        root.querySelector("input[name=sku]").value = meta?.sku || ""

        const sale = root.querySelector("#sale")
        const price = root.querySelector("#price")
        if (meta?.currency) sale.dataset.base = price.dataset.base = meta.currency
        if (meta?.sale) sale.dataset.amount = meta.sale
        if (meta?.price) price.dataset.amount = meta.price

        if (meta) this._renderGameData(meta)

        if (root.querySelector("#attributes").children.length === 0 && meta?.attributes?.length) {
            const attributes = meta.attributes.map(
                (attr) => html`
                    <fieldset>
                        <legend><ui-context data-key="dictionary.${attr.name}" /></legend>
                        ${attr.values.map(
                            (value) => html`
                                <input id="${value}" type="radio" name="${attr.name}" value="${value}" required="required" />
                                <label for="${value}"><ui-context data-key="dictionary.${value}" /></label>
                            `
                        )}
                    </fieldset>
                `
            )
            render(attributes, root.querySelector("#attributes"))
        }

        root.querySelector("#decrease").addEventListener("click", this.decrease)
        root.querySelector("#increase").addEventListener("click", this.increase)
        root.querySelector("#add").addEventListener("click", this.add)
        this.subscriptions.push(
            () => root.querySelector("#decrease").removeEventListener("click", this.decrease),
            () => root.querySelector("#increase").removeEventListener("click", this.increase),
            () => root.querySelector("#add").removeEventListener("click", this.add)
        )
    }
}

customElements.define("route-item", ITEM)

export default ITEM
