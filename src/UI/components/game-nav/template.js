import styles from "./styles.css.js"
import { html } from "/core/UI.js"

export const template = html`
    ${styles}
    <button class="game-nav__toggle" aria-label="Open game navigator">Games</button>
    <div class="game-nav__backdrop"></div>
    <nav class="game-nav__panel" aria-label="Game navigator">
        <div class="game-nav__header">
            <span class="game-nav__title">Games</span>
            <button class="game-nav__close" aria-label="Close">✕</button>
        </div>
        <div class="game-nav__list"></div>
    </nav>
`

export default template
