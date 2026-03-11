import { html } from "/core/UI.js"
import "/UI/layouts/main/index.js"
import "/UI/components/wallets/index.js"
import "/UI/components/context/index.js"
import "/UI/components/button/index.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <layout-main>
        <h1><ui-context data-key="dictionary.withdraw" /></h1>
        <main>
            <ui-wallets />
            <form id="form">
                <input type="number" name="amount" min="0" step="any" required disabled />
                <input type="text" name="address" required disabled />
                <div class="buttons">
                    <ui-button class="full" data-left="send" id="submit">
                        <ui-context data-key="dictionary.withdraw" />
                    </ui-button>
                </div>
            </form>
        </main>
    </layout-main>
`
export default template
