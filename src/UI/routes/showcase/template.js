import "/UI/layouts/main/index.js"
import styles from "./styles.css.js"
import { html } from "/core/UI.js"

export const template = html`
    ${styles}
    <layout-main>
        <div class="showcase">
            <aside id="sidebar"></aside>
            <section id="canvas"></section>
        </div>
    </layout-main>
`

export default template
