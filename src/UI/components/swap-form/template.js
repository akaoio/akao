import { html } from "/core/UI.js"
import "/UI/components/context/index.js"
import "/UI/components/label/index.js"
import "/UI/components/button/index.js"
import "/UI/components/icon/index.js"
import "/UI/components/modal/index.js"
import "/UI/components/option-list/index.js"
import "/UI/components/svg/index.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <div id="fields">
        <div class="token-field" id="sell-field">
            <ui-label data-key="dictionary.sell"></ui-label>
            <div class="field-row">
                <input type="number" id="amount-in" min="0" step="any" disabled placeholder="0" />
                <div class="token-picker" id="from-token">
                    <button class="token-trigger" id="from-trigger">
                        <ui-svg class="trigger-icon" />
                        <span class="trigger-placeholder"><ui-context data-key="dictionary.selectToken" /></span>
                        <span class="trigger-value"></span>
                    </button>
                    <ui-modal id="from-modal" data-header="selectToken">
                        <ui-option-list id="from-list"></ui-option-list>
                    </ui-modal>
                </div>
            </div>
            <div class="field-hint" id="balance-in"></div>
        </div>

        <div id="flip-row">
            <ui-icon id="flip-btn" data-icon="arrow-down-up" aria-label="Flip tokens" />
        </div>

        <div class="token-field" id="buy-field">
            <ui-label data-key="dictionary.buy"></ui-label>
            <div class="field-row">
                <div id="quote-out" class="quote-display">0</div>
                <div class="token-picker" id="to-token">
                    <button class="token-trigger" id="to-trigger">
                        <ui-svg class="trigger-icon" />
                        <span class="trigger-placeholder"><ui-context data-key="dictionary.selectToken" /></span>
                        <span class="trigger-value"></span>
                    </button>
                    <ui-modal id="to-modal" data-header="selectToken">
                        <ui-option-list id="to-list"></ui-option-list>
                    </ui-modal>
                </div>
            </div>
            <div class="field-hint" id="gas"></div>
        </div>
    </div>

    <div id="error"></div>

    <div id="slippage-row">
        <ui-label data-key="dictionary.slippage"></ui-label>
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
`

export default template
