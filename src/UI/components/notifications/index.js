import template from "./template.js"
import { html, render } from "/core/UI.js"
import { events } from "/core/Events.js"
import logic from "./logic.js"

export class NOTIFICATIONS extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
        this.close = this.close.bind(this)
    }

    close(key) {
        const notification = this.shadowRoot.querySelector(`.notification[data-key="${key}"]`)
        if (notification) this.shadowRoot.removeChild(notification)
    }

    connectedCallback() {
        this.subscriptions.push(
            events.on("notify", ({ detail }) => {
                const key = logic.key()
                const close = () => this.close(key)
                const notification = html`
                    <div class="notification" data-key="${key}">
                        <div class="content">${detail.content}</div>
                        <ui-icon
                            data-icon="x-lg"
                            ${({ element }) => {
                                element.addEventListener("click", close)
                                this.subscriptions.push(() => element.removeEventListener("click", close))
                            }} />
                    </div>
                `
                render(notification, this.shadowRoot, { append: true })
                if (detail?.autoClose === false) return
                setTimeout(close, detail?.delay || 5000)
            })
        )
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }
}

customElements.define("ui-notifications", NOTIFICATIONS)
