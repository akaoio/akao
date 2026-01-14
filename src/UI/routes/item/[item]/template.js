import "/UI/layouts/main/index.js"
import "/UI/components/context/index.js"
import "/UI/components/button/index.js"
import "/UI/components/icon/index.js"
import { html } from "/core/UI.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <layout-main>
        <form id="item">
            <div id="image" style="grid-area: image;">image</div>
            <header style="grid-area: header;"><ui-context data-key="item.name" /></header>
            <main style="grid-area: main;">
                <section id="description"><ui-context data-key="item.description" /></section>
                <section id="price"><ui-context data-key="item.price" /></section>
                <section id="attributes"></section>
            </main>
            <footer style="grid-area: footer;">
                <fieldset id="quantity">
                    <legend><ui-context data-key="dictionary.quantity" /></legend>
                    <ui-icon id="decrease" data-icon="dash-lg"></ui-icon>
                    <input name="quantity" type="number" value="1" min="1" required />
                    <ui-icon id="increase" data-icon="plus-lg"></ui-icon>
                </fieldset>
                <ui-button id="add"><ui-context data-key="dictionary.addToCart" /></ui-button>
            </footer>
            <input type="hidden" name="item" required />
            <input type="hidden" name="sku" required />
        </form>
    </layout-main>
`
export default template
