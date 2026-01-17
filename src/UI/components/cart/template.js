import styles from "./styles.css.js"
import { html } from "/core/UI.js"
import "/UI/components/icon/index.js"
import "/UI/components/modal/index.js"
import "/UI/components/context/index.js"

export const template = html`
    ${styles}
    <ui-icon data-icon="bag" />
    <ui-modal data-header="cart">
        <div id="items"></div>
        <div id="total">
            <ui-context data-key="dictionary.total" />
            :
            <ui-fiat />
        </div>
    </ui-modal>
`

export default template
