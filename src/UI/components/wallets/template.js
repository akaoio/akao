import "/UI/components/identicons/index.js"
import "/UI/components/context/index.js"
import styles from "./styles.css.js"
import { html } from "/core/UI.js"

export const template = html`
    ${styles}
    <div id="balance"></div>
    <div>
        <ui-identicons data-name="wallet" data-size="7">
            <ui-context data-key="dictionary.wallet" />
        </ui-identicons>
        <div id="address"></div>
    </div>
    <div id="currencies"></div>
    <div id="chains"></div>
`

export default template
