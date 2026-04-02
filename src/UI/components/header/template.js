import "/UI/components/a/index.js"
import "/UI/components/user/index.js"
import "/UI/components/svg/index.js"
import "/UI/components/icon/index.js"
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
                <ui-icon class="games" data-icon="controller" />
                <a is="ui-a" data-to="/pools"><ui-icon data-icon="coin" /></a>
                <a is="ui-a" data-to="/swap"><ui-icon data-icon="arrow-down-up" /></a>
                <ui-user />
            </div>
        </nav>
    </header>
`

export default template
