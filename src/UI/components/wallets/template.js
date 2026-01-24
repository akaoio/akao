import "/UI/components/modal/index.js"
import "/UI/components/identicon/index.js"
import "/UI/components/context/index.js"
import "/UI/components/icon/index.js"
import styles from "./styles.css.js"
import { html } from "/core/UI.js"

export const template = html`
    ${styles}
    <header>
        <ui-context data-key="dictionary.wallets" />
        <nav>
            <ui-icon id="increase" data-icon="plus-lg" />
            <ui-icon id="decrease" data-icon="dash-lg" />
        </nav>
    </header>
    <div id="wallets"></div>
`

export default template
