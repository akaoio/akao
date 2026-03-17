import icon from "/css/elements/icon.css.js"
import { css } from "/core/UI.js"

export const styles = css`
    ${icon}

    :host {
        button {
            font-family: var(--header-font);
            font-size: var(--text-xs);
            letter-spacing: 0.08em;
            text-transform: uppercase;
            padding: var(--space-1) var(--space-2);
            cursor: pointer;
            border: var(--border);
            background: transparent;
            color: var(--color);
        }
    }
`

export default styles
