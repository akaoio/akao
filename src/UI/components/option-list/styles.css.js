import { css } from "/core/UI.js"

export const styles = css`
    :host {
        display: block;
    }

    #list {
        list-style: none;
        padding: 0;
        margin: 0;
        min-width: 14rem;
    }

    li {
        display: flex;
        align-items: center;
        gap: var(--space-sm);
        padding: var(--space-sm) var(--space);
        cursor: pointer;
        font-family: var(--header-font);
        font-size: var(--text-sm);
        letter-spacing: 0.06em;
        text-transform: uppercase;
        color: var(--color);
        border-bottom: 1px solid color-mix(in hsl, var(--color) 8%, transparent);
        transition:
            background var(--speed),
            color var(--speed);

        &:last-child {
            border-bottom: none;
        }

        &:hover {
            background: var(--accent-info-surface);
            color: var(--accent-info);
        }

        &[data-selected="true"] {
            color: var(--accent-action);
            background: color-mix(in hsl, var(--accent-action) 6%, transparent);
            text-shadow: var(--glow-action);
        }
    }

    ui-svg {
        width: calc(var(--unit-4) + var(--unit-5));
        height: calc(var(--unit-4) + var(--unit-5));
        flex-shrink: 0;
    }

    .meta {
        margin-left: auto;
        font-size: var(--text-xs);
        letter-spacing: 0.08em;
        opacity: 0.5;
        flex-shrink: 0;
    }
`

export default styles
