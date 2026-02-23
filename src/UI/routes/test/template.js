import "/UI/layouts/main/index.js"
import "/UI/components/context/index.js"
import { html } from "/core/UI.js"
import "/UI/components/addresses/index.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <layout-main>
        <h1><ui-context data-key="dictionary.test" /></h1>
        <div id="container">
            <div>
                <input type="checkbox" name="billingAddress" />
            </div>
            <div>
                <ui-geo data-id="8615918" />
            </div>
        </div>
    </layout-main>
`
export default template
