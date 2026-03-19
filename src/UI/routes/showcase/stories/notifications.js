import "/UI/components/notifications/index.js"
import "/UI/components/button/index.js"
import { events } from "/core/Events.js"

export default {
    name: "Notifications",
    stories: [
        {
            name: "Toast — info",
            code: `<ui-button>Trigger notification</ui-button>
<ui-notifications></ui-notifications>`,
            setup(container) {
                container.querySelector("ui-button").addEventListener("click", () => {
                    events.emit("notify", { content: "Item added to cart!", autoClose: true, delay: 2500 })
                })
            }
        },
        {
            name: "Toast — long message",
            code: `<ui-button>Trigger long notification</ui-button>
<ui-notifications></ui-notifications>`,
            setup(container) {
                container.querySelector("ui-button").addEventListener("click", () => {
                    events.emit("notify", { content: "Your order #AK-00142 has been confirmed and is now being processed.", autoClose: true, delay: 4000 })
                })
            }
        }
    ]
}
