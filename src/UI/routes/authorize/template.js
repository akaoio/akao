import { html } from "/core/UI.js"
import "/UI/layouts/main/index.js"
import "/UI/components/authorize/index.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <layout-main>
        <ui-authorize />
    </layout-main>
`

export default template