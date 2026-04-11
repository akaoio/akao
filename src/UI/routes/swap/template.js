import { html } from "/core/UI.js"
import "/UI/layouts/main/index.js"
import "/UI/components/wallets/index.js"
import "/UI/components/context/index.js"
import "/UI/components/button/index.js"
import "/UI/components/icon/index.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <layout-main>
        <main>
            <ui-wallets data-currency="false" />
            <div id="swap-card">

                <div id="tab-bar">
                    <button class="tab active"><ui-context data-key="dictionary.swap" /></button>
                </div>

                <div id="tab-content-swap">
                    <div class="token-field" id="sell-field">
                        <div class="field-label"><ui-context data-key="dictionary.sell" /></div>
                        <div class="field-row">
                            <input type="number" id="amount-in" min="0" step="any" disabled placeholder="0" />
                            <div id="from-token"></div>
                        </div>
                        <div class="field-hint" id="balance-in"></div>
                    </div>

                    <div id="flip-row">
                        <ui-icon id="flip-btn" data-icon="arrow-down-up" aria-label="Flip tokens" />
                    </div>

                    <div class="token-field" id="buy-field">
                        <div class="field-label"><ui-context data-key="dictionary.buy" /></div>
                        <div class="field-row">
                            <div id="quote-out" class="quote-display">0</div>
                            <div id="to-token"></div>
                        </div>
                        <div class="field-hint" id="gas"></div>
                    </div>

                    <div id="error"></div>

                    <div id="slippage-row">
                        <span class="slippage-label"><ui-context data-key="dictionary.slippage" /></span>
                        <div class="slippage-inputs">
                            <button class="slippage-preset" data-value="0.1">0.1%</button>
                            <button class="slippage-preset" data-value="0.5">0.5%</button>
                            <button class="slippage-preset" data-value="1">1%</button>
                            <input type="number" id="slippage" value="0.5" min="0.1" max="50" step="0.1" />
                            <span class="slippage-unit">%</span>
                        </div>
                    </div>

                    <div class="actions">
                        <ui-button class="full" id="submit" disabled>
                            <ui-context data-key="dictionary.swap" />
                        </ui-button>
                    </div>
                </div>

            </div>
        </main>
    </layout-main>
`

export default template
