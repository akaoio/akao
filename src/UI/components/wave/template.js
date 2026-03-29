import { html } from "/core/UI.js"
import styles from "./styles.css.js"
import "/UI/components/visualizer/index.js"
import "/UI/components/context/index.js"

export const template = html`
    ${styles}
    <ui-context id="status" data-key="dictionary.idle" data-fallback="idle"/>
    <ui-visualizer/>
`

export default template