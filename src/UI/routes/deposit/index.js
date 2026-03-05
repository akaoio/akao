import template from "./template.js"
import { render } from "/core/UI.js"
import { Wallets } from "/core/Stores.js"
import {Access} from "/core/Access.js"

export class DEPOSIT extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }

    connectedCallback() {
        Access.on("authenticated", () => {
            console.log("AUT", Access.get("pair"), Access.get("wallet"))
            console.log("WALLET", Wallets)
        })
    }
}

customElements.define("route-deposit", DEPOSIT)

export default DEPOSIT
