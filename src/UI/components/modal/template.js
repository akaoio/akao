import styles from "./styles.css.js"
import { html } from "/core/UI.js"
import "/UI/components/context/index.js"

export const template = html`
    ${styles}
    <dialog>
        <div class="container">
            <header>
                <ui-context id="header" />
<slot name="header"></slot>
            </header>
            <section>
                <slot></slot>
            </section>
            <footer>
                <slot name="footer"></slot>
                <span class="icon close"></span>
            </footer>
        </div>
    </dialog>
`

export default template
