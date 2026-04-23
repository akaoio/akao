import "/UI/layouts/main/index.js"
import "/UI/components/context/index.js"
import "/UI/components/item/index.js"
import "/UI/components/loader/index.js"
import "/UI/components/search/index.js"
import "/UI/components/filter-group/index.js"
import "/UI/components/svg/index.js"
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
            <div class="catalog-pill" id="catalog-pill" role="group" aria-label="Filters summary">
                <span class="pill__count" id="pill-count">
                    <span class="count__num" id="pill-count-num"></span>
                    <span class="count__label">Items</span>
                </span>
                <span class="pill__divider"></span>
                <button class="pill__type active" id="pill-type" aria-label="Filter by type">All</button>
                <span class="pill__divider"></span>
                <span class="pill__rarity-dot" id="pill-rarity-dot" role="button" tabindex="0" aria-label="Filter by rarity"></span>
                <span class="pill__divider"></span>
                <span class="pill__sort" id="pill-sort"></span>
                <span class="pill__divider"></span>
                <span class="pill__search" id="pill-search" aria-label="Search">
                    <ui-svg data-src="/images/icons/search.svg"></ui-svg>
                </span>
            </div>
            <nav class="marketplace-nav">
                <ui-filter-group id="type-filter">
                    <span slot="label">Type</span>
                </ui-filter-group>
                <ui-filter-group id="rarity-filter" bordered data-pill>
                    <span slot="label">Rarity</span>
                </ui-filter-group>
            </nav>

            <div class="catalog-toolbar" id="toolbar">
                <span class="catalog-count" id="count">
                    <span class="count__num" id="count-num"></span>
                    <span class="count__label">Items</span>
                </span>
                <ui-search id="search" placeholder="Search items…"></ui-search>
                <div class="sort-bar" id="sort"></div>
                <button class="catalog-collapse-btn" id="catalog-collapse" aria-label="Collapse filters">
                    <ui-svg data-src="/images/icons/chevron-double-left.svg"></ui-svg>
                </button>
            </div>
        </div>

        <div class="catalog-grid-wrap" id="items-wrap">
            <div class="catalog-grid grid" id="items"></div>
            <p class="catalog-empty" id="items-empty" hidden></p>
            <ui-loader aria-live="polite" aria-label="Loading items">Loading…</ui-loader>
        </div>

        <div class="load-more-wrap">
            <button class="load-more-btn" id="load-more" hidden></button>
        </div>
    </layout-main>
`

export default template
