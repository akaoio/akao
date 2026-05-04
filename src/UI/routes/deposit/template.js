import { html } from "/core/UI.js"
import "/UI/layouts/main/index.js"
import "/UI/components/context/index.js"
import "/UI/components/qr/index.js"
import "/UI/components/svg/index.js"
import "/UI/components/icon/index.js"
import "/UI/components/card/index.js"
import "/UI/components/modal/index.js"
import "/UI/components/option-list/index.js"
import "/UI/components/button/index.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <layout-main>
        <main>
            <ui-card data-title="deposit">
                <div id="deposit-body">
                    <!-- ── Coin / Chain selection row ── -->
                    <div id="selection" hidden>
                        <div id="selection-coin">
                            <span class="selection-label"><ui-context data-key="dictionary.coin" /></span>
                            <button id="coin-badge" class="badge" aria-label="Change coin">
                                <span class="badge-body">
                                    <ui-svg id="coin-icon" class="badge-icon"></ui-svg>
                                    <span id="coin-name"></span>
                                </span>
                            </button>
                        </div>
                        <div id="selection-chain">
                            <span class="selection-label"><ui-context data-key="dictionary.network" /></span>
                            <button id="chain-badge" class="badge" aria-label="Change network">
                                <span class="badge-body">
                                    <ui-svg id="chain-icon" class="badge-icon"></ui-svg>
                                    <span id="chain-name"></span>
                                </span>
                            </button>
                        </div>
                    </div>

                    <!-- ── QR section ── -->
                    <div id="qr-section" hidden>
                        <p id="scan-label"><ui-context data-key="dictionary.scanToDeposit" /></p>
                        <div id="qr-frame">
                            <div id="qr-corner" aria-hidden="true">
                                <span class="corner tl"></span>
                                <span class="corner tr"></span>
                                <span class="corner bl"></span>
                                <span class="corner br"></span>
                            </div>
                            <ui-qr id="qr" />
                        </div>
                    </div>

                    <p id="chain-warning" hidden>
                        <svg class="warn-icon" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg" aria-hidden="true">
                            <path d="M12 2L1 21h22L12 2Z" stroke="currentColor" stroke-width="2" stroke-linejoin="round"/>
                            <line x1="12" y1="9" x2="12" y2="14" stroke="currentColor" stroke-width="2" stroke-linecap="round"/>
                            <circle cx="12" cy="18" r="1" fill="currentColor"/>
                        </svg>
                        <ui-context data-key="dictionary.wrongChainWarning" />
                    </p>

                    <!-- ── Address ── -->
                    <div id="address-wrap" hidden>
                        <div id="address-box">
                            <div id="address"></div>
                            <span id="copy-btn" aria-hidden="true">
                                <ui-icon data-icon="copy" />
                            </span>
                        </div>
                    </div>

                    <div id="deposit-actions">
                        <ui-button class="full" id="save-share">SAVE AND SHARE</ui-button>
                    </div>
                </div>
            </ui-card>
        </main>
    </layout-main>

    <!-- ── Coin picker modal ── -->
    <ui-modal id="coin-modal" data-header="select coin">
        <ui-option-list id="coin-list"></ui-option-list>
    </ui-modal>

    <!-- ── Chain picker modal ── -->
    <ui-modal id="chain-modal" data-header="choose network">
        <ui-option-list id="chain-list"></ui-option-list>
    </ui-modal>
`
export default template
