import "/UI/layouts/main/index.js"
import "/UI/components/context/index.js"
import "/UI/components/items/index.js"
import "/UI/components/button/index.js"
import "/UI/components/a/index.js"
import styles from "./styles.css.js"
import { html } from "/core/UI.js"

export const template = html`
    ${styles}
    <layout-main>

        <section class="hero">
            <div class="hero__eyebrow">
                <ui-context data-key="dictionary.home" />
            </div>
            <h1>AKAO</h1>
            <p class="hero__sub">Premium goods for the discerning buyer.</p>
            <div class="hero__actions">
                <ui-button data-right="arrow-right-short">Shop Now</ui-button>
            </div>
        </section>

        <section class="catalog">
            <div class="section-header">
                <h2>Featured</h2>
                <a is="ui-a" data-to="/inventory">View all →</a>
            </div>
            <ui-items data-columns="4" data-start="1" data-end="1" />
        </section>

        <section class="catalog">
            <div class="section-header">
                <h2>New Arrivals</h2>
                <a is="ui-a" data-to="/inventory">View all →</a>
            </div>
            <ui-items data-columns="5" data-start="2" data-end="3" />
        </section>

    </layout-main>
`
export default template
