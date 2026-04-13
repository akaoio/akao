import template from "./template.js"
import { Context } from "/core/Context.js"
import Router from "/core/Router.js"
import { html, render } from "/core/UI.js"
import { notify } from "/core/Utils.js"
import { States } from "/core/States.js"
import Cart from "/core/Cart.js"
import Route from "/core/UI/Route.js"
import logic from "./logic.js"

export class GAME_ITEM_ROUTE extends Route {
    static module = import.meta.url
    constructor() {
        super(template)
        const params = Context.get("params") || globalThis.history.state?.params || {}
        this.states = new States({ game: params.game, id: params.item })
        this.render = this.render.bind(this)
        this.increase = this.increase.bind(this)
        this.decrease = this.decrease.bind(this)
        this.add = this.add.bind(this)
    }

    async onconnect() {
        const game = this.states.get("game")
        const id = this.states.get("id")
        const meta = await logic.meta(game, id)
        if (!meta) {
            this.states.set({ meta: null, missing: true })
            this.render()
            return
        }
        this.states.set({ meta })
        this.sub(Context.on("locale", this.render))
        this.listen(this.shadowRoot.querySelector("#decrease"), "click", this.decrease)
        this.listen(this.shadowRoot.querySelector("#increase"), "click", this.increase)
        this.listen(this.shadowRoot.querySelector("#add"), "click", this.add)
        this.render()
    }

    ondisconnect() {
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

        const imageEl = root.querySelector("#icon")
        const firstImage = Array.isArray(meta.images) && meta.images.length ? meta.images[0] : null
        if (firstImage && meta.game && meta.id) {
            imageEl.src = `/statics/items/${meta.game}/${meta.id}/images/${firstImage}`
            imageEl.alt = ""
            imageEl.style.display = ""
        } else 
            imageEl.style.display = "none"
        

        const rarityKey = (meta.rarity || "common").toLowerCase().replace(/\s+/g, "-")
        const rarityColor = `var(--rarity-${rarityKey}, var(--color-accent, #888))`
        this.style.setProperty("--item-rarity-color", rarityColor)

        const rarityBadge = root.querySelector("#rarity-badge")
        rarityBadge.textContent = meta.rarity || ""
        rarityBadge.style.background = rarityColor
        rarityBadge.style.display = meta.rarity ? "" : "none"

        const typeBadge = root.querySelector("#type-badge")
        const displayType = logic.display(meta)
        typeBadge.textContent = displayType
        typeBadge.style.display = displayType ? "" : "none"

        const subtypeBadge = root.querySelector("#subtype-badge")
        subtypeBadge.textContent = meta.subtype || ""
        subtypeBadge.style.display = meta.subtype ? "" : "none"

        const backLink = root.querySelector("#back-link")
        if (meta.game) {
            backLink.textContent = meta.game.replace(/-/g, " ").replace(/\b\w/g, (c) => c.toUpperCase())
            backLink.dataset.to = `/game/${meta.game}`
            root.querySelector("#breadcrumb").style.display = ""
        } else 
            root.querySelector("#breadcrumb").style.display = "none"
        

        const flavorEl = root.querySelector("#flavor-text")
        flavorEl.textContent = meta.flavor_text || ""
        flavorEl.style.display = meta.flavor_text ? "" : "none"

        const statBlock = root.querySelector("#stat-block")
        if (statBlock.children.length === 0 && meta.stat_block) {
            const rows = Object.entries(meta.stat_block).map(([key, val]) =>
                html`<div class="stat-row"><dt>${logic.label(key)}</dt><dd>${val}</dd></div>`
            )
            render(rows, statBlock)
            root.querySelector("#stats").style.display = ""
        } else if (!meta.stat_block) 
            root.querySelector("#stats").style.display = "none"
        

        const slotsEl = root.querySelector("#loadout-slots")
        if (slotsEl.children.length === 0 && meta.loadout_slots?.length) {
            const chips = meta.loadout_slots.map((slot) =>
                html`<span class="slot-chip">${logic.label(slot)}</span>`
            )
            render(chips, slotsEl)
            root.querySelector("#slots").style.display = ""
        } else if (!meta.loadout_slots?.length) 
            root.querySelector("#slots").style.display = "none"
        
    }

    async render() {
        const game = this.states.get("game")
        const id = this.states.get("id")
        const meta = this.states.get("meta")
        if (!meta) {
            Router.setHead({ title: "Item not found", description: "" })
            Context.set({ item: { name: "Item not found", description: "This item is no longer available." } })

            const root = this.shadowRoot
            root.querySelector("#breadcrumb").style.display = game ? "" : "none"
            root.querySelector("#back-link").textContent = game ? game.replace(/-/g, " ").replace(/\b\w/g, (c) => c.toUpperCase()) : ""
            root.querySelector("#back-link").dataset.to = game ? `/game/${game}` : "/"
            root.querySelector("#breadcrumb-name").textContent = id || ""
            root.querySelector("#image").style.display = "none"
            root.querySelector(".badges").style.display = "none"
            root.querySelector("#flavor-text").style.display = "none"
            root.querySelector("#stats").style.display = "none"
            root.querySelector("#slots").style.display = "none"
            root.querySelector("#pricing").style.display = "none"
            root.querySelector("#attributes").style.display = "none"
            root.querySelector("footer").style.display = "none"
            return
        }
        const data = await logic.locale(game, id, Context.get("locale").code)
        if (!data) return
        this.states.set({ data })
        Router.setHead({
            title: data.name,
            description: data.description || ""
        })
        Context.set({ item: { ...meta, ...data } })

        const root = this.shadowRoot
        root.querySelector("#breadcrumb-name").textContent = data.name || ""
        root.querySelector("input[name=id]").value = `${game}/${id}`
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
    }
}

customElements.define("route-item-game-item", GAME_ITEM_ROUTE)

export default GAME_ITEM_ROUTE
