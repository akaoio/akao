import "/UI/layouts/main/index.js"
import "/UI/components/context/index.js"
import "/UI/components/button/index.js"
import "/UI/components/icon/index.js"
import "/UI/components/fiat/index.js"
import "/UI/components/a/index.js"
import { html } from "/core/UI.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <layout-main>
        <form id="item" class="grid--sidebar">
            <nav id="breadcrumb" style="grid-area: breadcrumb;">
                <button id="back-btn" type="button" class="back-btn">
                    <span class="back-arrow">←</span>
                    <span id="back-label"></span>
                </button>
            </nav>
            <div id="image" style="grid-area: image;">
                <img id="icon" alt="" loading="lazy" />
            </div>
            <header style="grid-area: header;">
                <div class="badges">
                    <span class="badge badge--rarity" id="rarity-badge"></span>
                    <span class="badge badge--type" id="type-badge"></span>
                    <span class="badge badge--type" id="subtype-badge"></span>
                </div>
                <h1><ui-context data-key="item.name" /></h1>
            </header>
            <main style="grid-area: main;">
                <p id="flavor-text"></p>
                <section id="description"><ui-context data-key="item.description" /></section>
                <section id="stats">
                    <dl id="stat-block"></dl>
                </section>
                <section id="slots">
                    <div id="loadout-slots"></div>
                </section>
                <section id="pricing">
                    <ui-fiat id="sale" />
                    <ui-fiat id="price" />
                </section>
                <section id="attributes"></section>
            </main>
            <footer style="grid-area: footer;">
                <fieldset id="quantity">
                    <legend><ui-context data-key="dictionary.quantity" /></legend>
                    <ui-icon id="decrease" data-icon="dash-lg"></ui-icon>
                    <input name="quantity" type="number" value="1" min="1" required="required" />
                    <ui-icon id="increase" data-icon="plus-lg"></ui-icon>
                </fieldset>
                <ui-button id="add"><ui-context data-key="dictionary.addToCart" /></ui-button>
            </footer>
            <input type="hidden" name="id" required="required" />
            <input type="hidden" name="sku" required="required" />
        </form>
    </layout-main>
`
export default template
