import styles from "./styles.css.js"
import { html } from "/core/UI.js"

export const template = html`
    ${styles}
    <button class="game-nav__toggle" aria-label="Open game navigator">
        <span class="game-nav__toggle-label">Games</span>
        <svg class="game-nav__toggle-icon" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true">
            <rect x="2" y="7" width="20" height="11" rx="3"/>
            <path d="M7 12.5h4M9 10.5v4"/>
            <circle cx="15.5" cy="11.5" r="1.2" fill="currentColor" stroke="none"/>
            <circle cx="17.5" cy="13.5" r="1.2" fill="currentColor" stroke="none"/>
        </svg>
    </button>
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
