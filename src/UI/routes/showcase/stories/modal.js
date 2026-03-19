import "/UI/components/modal/index.js"
import "/UI/components/button/index.js"

export default {
    name: "Modal",
    stories: [
        {
            name: "Non-modal (show)",
            code: `<ui-button>Open</ui-button>
<ui-modal data-header="home">
  <p class="modal-body">Non-blocking modal — page is still interactive.</p>
</ui-modal>`,
            setup(container) {
                const btn = container.querySelector("ui-button")
                const modal = container.querySelector("ui-modal")
                btn.addEventListener("click", () => modal.show())
            }
        },
        {
            name: "Modal dialog (showModal)",
            code: `<ui-button>Open Dialog</ui-button>
<ui-modal data-header="home">
  <p class="modal-body">Blocking modal dialog with backdrop.</p>
</ui-modal>`,
            setup(container) {
                const btn = container.querySelector("ui-button")
                const modal = container.querySelector("ui-modal")
                btn.addEventListener("click", () => modal.showModal())
            }
        },
        {
            name: "Centered",
            code: `<ui-button>Open Centered</ui-button>
<ui-modal class="center" data-header="home">
  <p class="modal-body">Centered modal dialog.</p>
</ui-modal>`,
            setup(container) {
                const btn = container.querySelector("ui-button")
                const modal = container.querySelector("ui-modal")
                btn.addEventListener("click", () => modal.showModal())
            }
        }
    ]
}
