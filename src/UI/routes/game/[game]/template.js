import "/UI/layouts/main/index.js"
import "/UI/components/context/index.js"
import "/UI/components/items/index.js"
import { html } from "/core/UI.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <layout-main>
        <header>
            <h1 id="name"></h1>
            <p id="description"></p>
        </header>
        <section id="meta"></section>
        <section id="items"></section>
    </layout-main>
`

export default template
