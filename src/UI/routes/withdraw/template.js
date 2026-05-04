import { html } from "/core/UI.js"
import "/UI/layouts/main/index.js"
import "/UI/components/context/index.js"
import "/UI/components/label/index.js"
import "/UI/components/svg/index.js"
import "/UI/components/card/index.js"
import "/UI/components/modal/index.js"
import "/UI/components/button/index.js"
import "/UI/components/wallets/index.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <layout-main>
        <main>
            <ui-card data-title="wallet">
                <ui-wallets id="wallets" data-no-persist data-prompt-chain></ui-wallets>
            </ui-card>
            <ui-card id="withdraw-card" data-title="withdraw">
                <!-- ── Send form ── -->
                <form id="form">
                    <div class="field-wrap">
                        <label class="field-label">
                            <ui-label data-key="dictionary.address"></ui-label>
                            <div class="address-input-wrap">
                                <div class="address-display" id="address-display" contenteditable="plaintext-only" spellcheck="false" autocomplete="off" role="textbox" aria-label="address"></div>
                                <input type="hidden" name="address" id="address-hidden" required />
                            </div>
                        </label>
                        <p id="address-error">
                            <ui-context data-key="dictionary.addressRequired" />
                        </p>
                    </div>
                    <label class="field-label">
                        <div class="amount-label-row">
                            <ui-label data-key="dictionary.amount"></ui-label>
                            <span id="available-balance"></span>
                        </div>
                        <div class="amount-input-row">
                            <input type="number" name="amount" min="0" step="any" required />
                            <span id="amount-symbol"></span>
                        </div>
                        <p id="amount-error">
                            <ui-context data-key="dictionary.insufficientBalance" />
                        </p>
                    </label>
                    <ul class="disclaimers">
                        <li>
                            <ui-svg class="disclaimer-icon" data-src="/images/icons/exclamation-triangle-fill.svg"></ui-svg>
                            <ui-context data-key="dictionary.wrongChainWarning" />
                        </li>
                        <li>
                            <ui-svg class="disclaimer-icon" data-src="/images/icons/arrow-counterclockwise.svg"></ui-svg>
                            <ui-context data-key="dictionary.withdrawIrreversible" />
                        </li>
                        <li>
                            <ui-svg class="disclaimer-icon" data-src="/images/icons/shield-check.svg"></ui-svg>
                            <ui-context data-key="dictionary.withdrawVerifyAddress" />
                        </li>
                    </ul>
                    <div class="form-actions">
                        <div id="gas"></div>
                        <ui-button class="full" id="submit" disabled>
                            <ui-context data-key="dictionary.withdraw" />
                        </ui-button>
                    </div>
                </form>
            </ui-card>
        </main>
    </layout-main>
`
export default template
