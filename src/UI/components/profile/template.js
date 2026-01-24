import { html } from "/core/UI.js"
import "/UI/components/modal/index.js"
import "/UI/components/signout/index.js"
import "/UI/components/wallets/index.js"
import "/UI/components/addresses/index.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <ui-modal data-header="profile" class="center">
        <ui-signout slot="header" />
        <section>
            <ui-wallets />
            <ui-addresses />
        </section>
    </ui-modal>
`

export default template