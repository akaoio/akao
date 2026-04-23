import { css } from "/core/UI.js"

export const styles = css`
    :host {
        display: block;
        position: relative;
    }

    .search-wrap {
        position: relative;
    }

    .search-input {
        width: 100%;
        background: transparent;
        border: 1px solid color-mix(in hsl, var(--color) 35%, transparent);
        color: var(--color);
        font-family: var(--header-font);
        font-size: var(--text-sm);
        letter-spacing: 0.06em;
        padding: var(--space-2) var(--space-3);
        outline: none;
        transition:
            border-color var(--speed),
            box-shadow var(--speed),
            background var(--speed);

        &::placeholder {
            color: var(--color);
            opacity: 0.2;
        }

        &::-webkit-search-cancel-button {
            filter: invert(0.5);
            cursor: pointer;
        }

        &:focus {
            border-color: var(--accent-info);
            background: color-mix(in hsl, var(--accent-info) 4%, transparent);
            box-shadow: inset 0 0 0 1px color-mix(in hsl, var(--accent-info) 30%, transparent);
        }

        &:not(:placeholder-shown) {
            border-color: color-mix(in hsl, var(--accent-info) 55%, transparent);
        }
    }

    /* ── Suggestions dropdown ── */
    .search-suggestions {
        position: absolute;
        top: calc(100% + 4px);
        left: 0;
        right: 0;
        z-index: var(--z-dropdown);
        list-style: none;
        background: color-mix(in hsl, var(--background) 96%, var(--accent-info));
        border: 1px solid color-mix(in hsl, var(--accent-info) 40%, transparent);
        box-shadow:
            0 8px 32px color-mix(in hsl, var(--background) 60%, transparent),
            0 0 0 1px color-mix(in hsl, var(--accent-info) 10%, transparent);
        max-height: 280px;
        overflow-y: auto;
        opacity: 0;
        transform: translateY(-6px);
        pointer-events: none;
        transition:
            opacity var(--speed),
            transform var(--speed);
        scrollbar-width: thin;
        scrollbar-color: var(--accent-info) transparent;

        &.open {
            opacity: 1;
            transform: translateY(0);
            pointer-events: auto;
        }
    }

    .search-suggestion {
        display: flex;
        align-items: center;
        justify-content: space-between;
        gap: var(--space-2);
        padding: var(--space-2) var(--space-3);
        cursor: pointer;
        transition: background var(--speed);

        &:not(:last-child) {
            border-bottom: 1px solid color-mix(in hsl, var(--color) 6%, transparent);
        }

        &:hover,
        &.highlighted {
            background: var(--accent-info-surface);
        }
    }

    .suggestion__name {
        font-family: var(--header-font);
        font-size: var(--text-xs);
        letter-spacing: 0.05em;
        color: var(--color);
        white-space: nowrap;
        overflow: hidden;
        text-overflow: ellipsis;
    }

    .suggestion__meta {
        flex-shrink: 0;
        font-family: var(--header-font);
        font-size: var(--text-xs);
        letter-spacing: 0.06em;
        text-transform: uppercase;
        color: var(--suggestion-meta-color, var(--color-accent));
        opacity: 0.8;
    }
`

export default styles
