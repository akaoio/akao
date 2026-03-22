import "/UI/components/a/index.js"
import "/UI/components/cart/index.js"
import "/UI/components/user/index.js"
import "/UI/components/svg/index.js"
import styles from "./styles.css.js"
import { html } from "/core/UI.js"
import { Statics } from "/core/Stores.js"

export const template = html`
    ${styles}
    <header>
        <nav>
            <a is="ui-a" data-to="/">
                <ui-svg class="brand" data-src="${Statics?.site?.brand?.text}" />
            </a>
            <div>
                <button class="game-nav-btn" aria-label="Open game navigator">
                    <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true">
                        <rect x="2" y="7" width="20" height="11" rx="3"/>
                        <path d="M7 12.5h4M9 10.5v4"/>
                        <circle cx="15.5" cy="11.5" r="1.2" fill="currentColor" stroke="none"/>
                        <circle cx="17.5" cy="13.5" r="1.2" fill="currentColor" stroke="none"/>
                    </svg>
                </button>
                <ui-cart />
                <ui-user />
            </div>
        </nav>
    </header>
`

export default template
