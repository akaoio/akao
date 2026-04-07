import { html } from "/core/UI.js"
import "/UI/layouts/main/index.js"
import "/UI/components/context/index.js"
import "/UI/components/pool/index.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <layout-main>

        <section class="page-header">
            <div class="page-header__title-row">
                <h1 class="page-header__title">
                    <ui-context data-key="dictionary.pools" />
                </h1>
                <span class="live-indicator" id="live-dot" hidden aria-label="Live data"></span>
            </div>
            <p class="page-header__meta">
                <span class="loading-status" id="loading-status">Preparing pool data…</span>
                <span class="pool-count" id="pool-count" hidden>0</span><span class="pool-count__label" id="pool-count-label" hidden> pools</span>
            </p>
        </section>

        <div id="loader" aria-label="Loading pools">
            <div class="loader-spinner"></div>
        </div>

        <div class="pools-content" hidden>
        <div class="sticky-sentinel"></div>

        <div class="filter-strip" id="filter-strip">
            <div class="filter-group" id="filter-chains">
                <span class="filter-group__label">Chain</span>
                <span class="filter-select-wrap">
                    <select class="filter-select" id="chain-select"></select>
                </span>
            </div>
            <div class="filter-group" id="filter-dexes">
                <span class="filter-group__label">DEX</span>
                <div class="filter-pills" id="dex-pills"></div>
            </div>
        </div>

        <div class="table-header" aria-hidden="true">
            <span class="th th-pair">Pool</span>
            <span class="th th-tvl">TVL</span>
            <span class="th th-rate">Rate</span>
            <span class="th th-badges">Protocol</span>
        </div>

        <div id="skeleton" class="skeleton-list" aria-hidden="true">
            <div class="skeleton-row"></div>
            <div class="skeleton-row"></div>
            <div class="skeleton-row"></div>
            <div class="skeleton-row"></div>
            <div class="skeleton-row"></div>
        </div>

        <div id="list" role="list"></div>

        <div id="empty" hidden class="empty-state">
            <span class="empty-state__icon">⬡</span>
            <span class="empty-state__text"></span>
        </div>

        <div class="load-more-wrap">
            <button class="load-more-btn" id="load-more" hidden>
                <span id="load-more-label">Load more</span>
            </button>
        </div>

        </div><!-- /.pools-content -->

    </layout-main>
`

export default template
