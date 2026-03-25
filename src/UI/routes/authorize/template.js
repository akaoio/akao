import { html } from "/core/UI.js"
import "/UI/layouts/main/index.js"
import "/UI/components/camera/index.js"
import "/UI/components/icon/index.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <layout-main data-footer="false">
        <ui-camera></ui-camera>
    </layout-main>
`

export default template