import button from "/css/elements/button.css.js"
import input from "/css/elements/input.css.js"
import scrollbar from "/css/elements/scrollbar.css.js"
import { css } from "/core/UI.js"

export const styles = css`
    ${button}
    ${input}
    ${scrollbar}
    :host {
        display: inline-block;
        position: relative;
        button.trigger {
            min-width: 140px;
        }
        button.trigger ui-svg {
            width: var(--icon-sm);
            height: var(--icon-sm);
            flex-shrink: 0;
        }
        .dropdown {
            position: absolute;
            top: calc(100% + var(--space-sm));
            left: 0;
            min-width: 220px;
            background: var(--background);
            border: var(--border);
            border-radius: var(--radius);
            z-index: 100;
            overflow: hidden;
        }
        .dropdown[hidden] {
            display: none;
        }
        input.search {
            border-bottom: var(--border);
            border-top: none;
            border-left: none;
            border-right: none;
            border-radius: 0;
            width: 100%;
        }
        .options {
            max-height: 240px;
            overflow-y: auto;
        }
        .option {
            display: flex;
            align-items: center;
            gap: var(--space);
            padding: var(--space);
            cursor: pointer;
            font-size: var(--text-sm);
        }
        .option:hover {
            background: var(--background-inverted);
            color: var(--color-inverted);
        }
        .option ui-svg {
            width: var(--icon-sm);
            height: var(--icon-sm);
            flex-shrink: 0;
        }
    }
`

export default styles
