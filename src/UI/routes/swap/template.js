import { html } from "/core/UI.js"
import "/UI/layouts/main/index.js"
import "/UI/components/wallets/index.js"
import "/UI/components/card/index.js"
import "/UI/components/swap-form/index.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <layout-main>
        <main>
            <ui-card data-title="wallet">
                <ui-wallets data-currency="false" data-no-persist />
            </ui-card>
            <ui-card id="swap-card" data-title="swap">
                <ui-swap-form id="swap-form" />
            </ui-card>
        </main>
    </layout-main>
`

export default template
