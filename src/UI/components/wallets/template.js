import "/UI/components/modal/index.js"
import "/UI/components/identicon/index.js"
import "/UI/components/context/index.js"
import "/UI/components/icon/index.js"
import styles from "./styles.css.js"
import { html } from "/core/UI.js"

export const template = html`
    ${styles}
    <div>
        <header>
            <ui-context data-key="dictionary.wallet" />
            <nav>
                <ui-icon id="increase" data-icon="plus-lg" />
                <ui-icon id="decrease" data-icon="dash-lg" />
            </nav>
        </header>
        <div id="wallets"></div>
    </div>
    <div>
        <header>
            <ui-context data-key="dictionary.chain" />
        </header>
        <div id="chains"></div>
    </div>
    <div>
        <header>
            <ui-context data-key="dictionary.currency" />
        </header>
        <div id="currencies"></div>
    </div>
    <div>
        <header>
            <ui-context data-key="dictionary.address" />
        </header>
        <div id="address"></div>
    </div>
`

export default template
