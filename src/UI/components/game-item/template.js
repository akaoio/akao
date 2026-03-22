import { html } from "/core/UI.js"
import styles from "./styles.css.js"
import "/UI/components/a/index.js"

export const template = html`
    ${styles}
    <div class="badges">
        <span class="badge badge--rarity" id="rarity-badge"></span>
        <span class="badge badge--type" id="type-badge"></span>
        <span class="badge badge--type" id="subtype-badge"></span>
    </div>
    <div class="body">
        <div class="icon" id="icon-wrap">
            <svg class="icon__placeholder" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
                <rect x="3" y="3" width="18" height="18" rx="2" stroke="currentColor" stroke-width="1.5" />
                <path d="M3 9h18M9 21V9" stroke="currentColor" stroke-width="1.5" />
            </svg>
        </div>
        <div class="info">
            <div class="name">
                <a is="ui-a" id="item-link"></a>
            </div>
            <div class="footer">
                <span class="price" id="price"></span>
            </div>
        </div>
    </div>
`

export default template
