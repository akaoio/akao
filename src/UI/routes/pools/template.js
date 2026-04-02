import { html } from "/core/UI.js"
import "/UI/layouts/main/index.js"
import "/UI/components/context/index.js"
import "/UI/components/pool/index.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <layout-main>
        <h1><ui-context data-key="dictionary.pools" /></h1>
        <main>
            <div id="filters"></div>
            <div id="list"></div>
            <div id="empty" hidden></div>
        </main>
    </layout-main>
`

export default template
