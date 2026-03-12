import "/UI/components/identicons/index.js"
import "/UI/components/context/index.js"
import styles from "./styles.css.js"
import { html } from "/core/UI.js"

export const template = html`
    ${styles}
    <ui-identicons data-name="avatar" data-size="7">
        <ui-context data-key="dictionary.avatars" />
    </ui-identicons>
`

export default template
