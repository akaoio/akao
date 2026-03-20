import "/UI/layouts/main/index.js"
import "/UI/components/context/index.js"
import "/UI/components/game-item/index.js"
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

        <nav class="marketplace-nav">
            <div class="filter-group">
                <span class="filter-group__label">Type</span>
                <div class="type-tabs" id="type-tabs"></div>
            </div>
            <div class="filter-group">
                <span class="filter-group__label">Rarity</span>
                <div class="rarity-pills" id="rarity-pills"></div>
            </div>
        </nav>

        <div class="catalog-toolbar" id="toolbar">
            <span class="catalog-count" id="count"></span>
            <div class="catalog-search-wrap">
                <input class="catalog-search" id="search" type="search" placeholder="Search items…" autocomplete="off" spellcheck="false" />
                <ul class="catalog-search__suggestions" id="search-suggestions"></ul>
            </div>
            <div class="sort-bar" id="sort"></div>
        </div>

        <div class="catalog-grid" id="items"></div>

        <div class="load-more-wrap">
            <button class="load-more-btn" id="load-more" hidden></button>
        </div>
    </layout-main>
`

export default template
