import template from "./template.js"
import { html, render } from "/core/UI.js"
import { events } from "/core/Events.js"
import BaseElement from "/UI/BaseElement.js"
import logic from "./logic.js"

export class NOTIFICATIONS extends BaseElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.close = this.close.bind(this)
    }

    close(key) {
        const notification = this.shadowRoot.querySelector(`.notification[data-key="${key}"]`)
        if (notification) this.shadowRoot.removeChild(notification)
    }

    onConnect() {
        this.subscribe(
            events.on("notify", ({ detail }) => {
                const key = logic.key()
                const close = () => this.close(key)
                const notification = html`
                    <div class="notification" data-key="${key}">
                        <div class="content">${detail.content}</div>
                        <ui-icon
                            data-icon="x-lg"
                            ${({ element }) => {
                                this.listen(element, "click", close)
                            }} />
                    </div>
                `
                render(notification, this.shadowRoot, { append: true })
                if (detail?.autoClose === false) return
                setTimeout(close, detail?.delay || 5000)
            })
        )
    }
}

customElements.define("ui-notifications", NOTIFICATIONS)
