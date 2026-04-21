import { html } from "/core/UI.js"
import "/UI/layouts/main/index.js"
import "/UI/components/context/index.js"
import "/UI/components/qr/index.js"
import "/UI/components/svg/index.js"
import "/UI/components/icon/index.js"
import "/UI/components/card/index.js"
import "/UI/components/modal/index.js"
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
                            <button id="coin-badge" class="badge" aria-label="Change coin">
                                <ui-svg id="coin-icon" class="badge-icon"></ui-svg>
                                <span id="coin-name"></span>
                                <ui-svg class="badge-chevron" data-src="/images/icons/chevron-down.svg"></ui-svg>
                            </button>
                        </div>
                        <div id="selection-chain">
                            <span class="selection-label"><ui-context data-key="dictionary.chain" /></span>
                            <button id="chain-badge" class="badge" aria-label="Change network">
                                <ui-svg id="chain-icon" class="badge-icon"></ui-svg>
                                <span id="chain-name"></span>
                                <ui-svg class="badge-chevron" data-src="/images/icons/chevron-down.svg"></ui-svg>
                            </button>
                            <p id="chain-warning">
                                <ui-context data-key="dictionary.wrongChainWarning" />
                            </p>
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

                    <!-- ── Address ── -->
                    <div id="address-wrap" hidden>
                        <div id="address-box">
                            <div id="address"></div>
                            <span id="copy-btn" aria-hidden="true">
                                <ui-icon data-icon="copy" />
                            </span>
                        </div>
                    </div>
                </div>
            </ui-card>
        </main>
    </layout-main>

    <!-- ── Coin picker modal ── -->
    <ui-modal id="coin-modal" data-header="currency">
        <ul id="coin-list"></ul>
    </ui-modal>

    <!-- ── Chain picker modal ── -->
    <ui-modal id="chain-modal" data-header="chain">
        <ul id="chain-list"></ul>
    </ui-modal>
`
export default template
