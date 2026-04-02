import { html } from "/core/UI.js"
import "/UI/layouts/main/index.js"
import "/UI/components/wallets/index.js"
import "/UI/components/context/index.js"
import "/UI/components/button/index.js"
import "/UI/components/token-select/index.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <layout-main>
        <h1><ui-context data-key="dictionary.swap" /></h1>
        <main>
            <ui-wallets />
            <div class="field">
                <label><ui-context data-key="dictionary.from" /></label>
                <div class="row">
                    <ui-token-select id="from-token"></ui-token-select>
                    <input type="number" id="amount-in" min="0" step="any" disabled />
                </div>
                <div class="info" id="balance-in"></div>
            </div>
            <div class="field">
                <label><ui-context data-key="dictionary.to" /></label>
                <div class="row">
                    <ui-token-select id="to-token"></ui-token-select>
                    <div id="quote-out"></div>
                </div>
            </div>
            <div id="slippage-row">
                <span><ui-context data-key="dictionary.slippage" />:</span>
                <input type="number" id="slippage" value="0.5" min="0.1" max="50" step="0.1" />
                <span>%</span>
            </div>
            <div id="gas"></div>
            <div id="error"></div>
            <div class="buttons">
                <ui-button class="full" id="submit" disabled>
                    <ui-context data-key="dictionary.swap" />
                </ui-button>
            </div>
        </main>
    </layout-main>
`

export default template
