import "/UI/components/label/index.js"
import "/UI/components/modal/index.js"
import "/UI/components/svg/index.js"
import "/UI/components/context/index.js"
import "/UI/components/option-list/index.js"
import styles from "./styles.css.js"
import { html } from "/core/UI.js"

export const template = html`
    ${styles}

    <div id="currency-row" hidden>
        <ui-label id="currency-label" data-key="dictionary.coin"></ui-label>
        <button class="trigger" id="currency-trigger">
            <span class="trigger-body">
                <ui-svg class="trigger-icon" id="currency-icon"></ui-svg>
                <span class="trigger-placeholder" id="currency-placeholder"></span>
                <span class="trigger-value" id="currency-name"></span>
            </span>
        </button>
    </div>

    <div id="chain-row" hidden>
        <ui-label id="chain-label" data-key="dictionary.network"></ui-label>
        <button class="trigger" id="chain-trigger">
            <span class="trigger-body">
                <ui-svg class="trigger-icon" id="chain-icon"></ui-svg>
                <span class="trigger-placeholder" id="chain-placeholder"></span>
                <span class="trigger-value" id="chain-name"></span>
            </span>
        </button>
    </div>

    <ui-modal id="currency-modal" data-header="currency">
        <ui-option-list id="currency-list"></ui-option-list>
    </ui-modal>

    <ui-modal id="chain-modal" data-header="chain">
        <ui-option-list id="chain-list"></ui-option-list>
    </ui-modal>
`

export default template
