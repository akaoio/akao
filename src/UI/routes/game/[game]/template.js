import "/UI/layouts/main/index.js"
import "/UI/components/context/index.js"
import "/UI/components/item/index.js"
import "/UI/components/loader/index.js"
import { html } from "/core/UI.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <layout-main>
        <header class="game-hero">
            <div class="game-hero__eyebrow" id="eyebrow"></div>
            <h1 id="name"></h1>
            <p id="description"></p>
            <div class="game-hero__meta" id="meta"></div>
        </header>

        <div class="sticky-sentinel"></div>
        <div class="catalog-sticky">
            <div class="catalog-pill" id="catalog-pill" aria-label="Open filters">
                <span class="pill__type" id="pill-type">All</span>
                <span class="pill__divider"></span>
                <span class="pill__rarity-dot" id="pill-rarity-dot"></span>
                <span class="pill__divider"></span>
                <span class="pill__count" id="pill-count"><span class="count__num" id="pill-count-num"></span><span class="count__label"> Items</span></span>
                <span class="pill__divider"></span>
                <span class="pill__sort" id="pill-sort"></span>
                <span class="pill__divider"></span>
                <span class="pill__search" id="pill-search" aria-label="Search">
                    <svg width="16" height="16" viewBox="0 0 16 16" fill="none">
                        <circle cx="6.5" cy="6.5" r="4.5" stroke="currentColor" stroke-width="1.5"/>
                        <line x1="10" y1="10" x2="14" y2="14" stroke="currentColor" stroke-width="1.5" stroke-linecap="round"/>
                    </svg>
                </span>
            </div>
            <nav class="marketplace-nav">
                <div class="filter-group">
                    <span class="filter-group__label">Type</span>
                    <div class="type-tabs" id="type-tabs"></div>
                    <span class="filter-select-wrap"><select class="filter-select" id="type-select"></select></span>
                </div>
                <div class="filter-group">
                    <span class="filter-group__label">Rarity</span>
                    <div class="rarity-pills" id="rarity-pills"></div>
                    <span class="filter-select-wrap"><select class="filter-select" id="rarity-select"></select></span>
                </div>
            </nav>

            <div class="catalog-toolbar" id="toolbar">
                <span class="catalog-count" id="count"><span class="count__num" id="count-num"></span><span class="count__label"> Items</span></span>
                <div class="catalog-search-wrap">
                    <input class="catalog-search" id="search" type="search" placeholder="Search items…" autocomplete="off" spellcheck="false" />
                    <ul class="catalog-search__suggestions" id="search-suggestions"></ul>
                </div>
                <div class="sort-bar" id="sort"></div>
                <button class="catalog-collapse-btn" id="catalog-collapse" aria-label="Collapse filters"></button>
            </div>
        </div>

        <div class="catalog-grid-wrap" id="items-wrap">
            <div class="catalog-grid" id="items"></div>
            <ui-loader aria-live="polite" aria-label="Loading items">Loading…</ui-loader>
        </div>

        <div class="load-more-wrap">
            <button class="load-more-btn" id="load-more" hidden></button>
        </div>
    </layout-main>
`

export default template
