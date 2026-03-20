import "/UI/components/header/index.js"
import "/UI/components/footer/index.js"
import "/UI/components/game-nav/index.js"
import styles from "./styles.css.js"
import { html } from "/core/UI.js"

export const template = html`
    ${styles}
    <game-nav></game-nav>
    <ui-header />
    <main><slot></slot></main>
    <ui-footer />
`

export default template
