import "/UI/components/modal/index.js"
import "/UI/components/context/index.js"
import "/UI/components/svg/index.js"
import styles from "./styles.css.js"
import { html } from "/core/UI.js"

export const template = html`
    ${styles}
    <ui-modal>
        <span slot="header" class="search-slot">
            <input class="picker-search" type="text" autocomplete="off" spellcheck="false" />
        </span>
    </ui-modal>
`

export default template
