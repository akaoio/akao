import { html } from "/core/UI.js"
import "/UI/layouts/main/index.js"
import "/UI/components/wallets/index.js"
import "/UI/components/context/index.js"
import "/UI/components/qr/index.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <layout-main>
        <h1><ui-context data-key="dictionary.deposit" /></h1>
        <main>
            <ui-wallets />
            <ui-qr id="qr" />
        </main>
    </layout-main>
`
export default template
