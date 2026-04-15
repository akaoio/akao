import "/UI/components/identicons/index.js"
import "/UI/components/icon/index.js"
import "/UI/components/modal/index.js"
import "/UI/components/svg/index.js"
import "/UI/components/context/index.js"
import styles from "./styles.css.js"
import { html } from "/core/UI.js"

export const template = html`
    ${styles}

    <!-- ── Wallet selector carousel ─────────────────────────────────────────────
         data-size="5" gives a 5×5 pixel grid — bold, readable cells at icon-lg
         display size (~48px). Bolder than 7×7 which blurs at that scale.

         slot[name="actions"] holds the name input (full-width, always visible)
         and the remove button. Clicking an identicon immediately commits it.
    ── -->
    <ui-identicons data-name="wallet" data-size="5" data-mode="picker">
        <span slot="actions" id="wallet-actions">
            <!-- Wallet rename: input only — confirm button lives in preview-actions slot -->
            <span id="label-wrap">
                <input id="label-input" type="text" maxlength="24" autocomplete="off" spellcheck="false" />
            </span>

            <!-- Right-side action buttons — always occupying the same slot:
                 • Resting:  nothing shown (no preview, not editing)
                 • Preview:  DELETE + SWAP
                 • Editing:  DELETE (if preview) + SAVE (replaces SWAP position)
            -->
            <span id="wallet-preview-actions" hidden>
                <button id="wallet-remove" aria-label="Delete wallet" hidden>
                    <ui-svg data-src="/images/icons/trash.svg" />
                </button>
                <button id="wallet-switch" aria-label="Switch to wallet">
                    <ui-svg data-src="/images/icons/arrow-left-right.svg" />
                </button>
                <button id="label-confirm" aria-label="Save wallet name" hidden>
                    <ui-svg data-src="/images/icons/check2.svg" />
                </button>
            </span>
        </span>
    </ui-identicons>

    <div id="wallet-row">
        <div id="address-wrap">
            <span id="address-half">
                <span id="wallet-num" aria-label="Wallet number"></span>
                <div id="address" title=""></div>
                <button id="copy-btn" aria-label="Copy address" disabled>
                    <ui-icon data-icon="copy" />
                </button>
            </span>
            <span id="address-divider" aria-hidden="true"></span>
            <span id="balance-wrap">
                <span id="balance" class="balance-num"></span>
                <span id="balance-symbol" class="balance-symbol"></span>
            </span>
        </div>
        <button class="chain-trigger" id="chain-trigger">
            <ui-svg class="trigger-icon" id="chain-icon"></ui-svg>
            <span class="trigger-placeholder"><ui-context data-key="dictionary.chain" /></span>
            <span class="trigger-value" id="chain-name"></span>
        </button>
    </div>

    <!-- ── Currency selector (deposit/withdraw routes only) ── -->
    <div id="currencies"></div>

    <!-- ── Chain picker modal ── -->
    <ui-modal id="chain-modal" data-header="chain">
        <ul id="chain-list"></ul>
    </ui-modal>

    <!-- ── Delete wallet confirm modal ── -->
    <ui-modal id="delete-modal" data-header="confirm">
        <p id="delete-modal-body"></p>
        <span slot="footer" id="delete-modal-footer">
            <button id="delete-confirm" class="icon">
                <ui-svg data-src="/images/icons/trash.svg" />
            </button>
        </span>
    </ui-modal>
`

export default template
