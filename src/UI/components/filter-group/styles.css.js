import { css } from "/core/UI.js"
import { mq } from "/UI/css/layout/breakpoints.js"

export const styles = css`
    :host {
        display: block;
    }

    .filter-group {
        display: flex;
        align-items: flex-start;
        gap: var(--space-3);
        padding: var(--space-2) var(--space-3);
    }

    :host([bordered]) .filter-group {
        border-top: 1px solid color-mix(in hsl, var(--color) 8%, transparent);
    }

    .filter-group__label {
        flex-shrink: 0;
        width: 3.5rem;
        padding-top: calc(var(--space-1) + 1px);
        font-family: var(--header-font);
        font-size: var(--text-xs);
        letter-spacing: 0.1em;
        text-transform: uppercase;
        color: var(--color);
        opacity: 0.35;
    }

    .filter-choices {
        flex: 1;
        min-width: 0;
    }

    /* ── Tabs (default pill/button style, desktop) ── */
    .filter-tabs {
        display: flex;
        flex-wrap: wrap;
        gap: var(--space-1);

        /* hide overflow buttons when not expanded */
        &:not(.expanded) [data-overflow] {
            display: none;
        }
    }

    /* ── Shared button style; consumers can opt into pill shape via data-pill ── */
    ::slotted(button),
    .filter-tabs button {
        font-family: var(--header-font);
        font-size: var(--text-xs);
        letter-spacing: 0.08em;
        text-transform: uppercase;
        padding: var(--space-1) var(--space-3);
        border: 1px solid var(--border);
        background: transparent;
        color: var(--color);
        cursor: pointer;
        transition:
            border-color var(--speed),
            color var(--speed),
            background var(--speed),
            box-shadow var(--speed);

        &:hover {
            border-color: var(--filter-accent, var(--accent-info));
            color: var(--filter-accent, var(--accent-info));
        }

        &.active {
            border-color: var(--filter-accent, var(--accent-info));
            color: var(--filter-accent, var(--accent-info));
            background: color-mix(in hsl, var(--filter-accent, var(--accent-info)) 6%, transparent);
            box-shadow: 0 0 12px color-mix(in hsl, var(--filter-accent, var(--accent-info)) 25%, transparent);
        }
    }

    /* Pill shape variant — applied when host has data-pill attribute */
    :host([data-pill]) .filter-tabs button {
        border-radius: 999px;
        padding: var(--space-1) var(--space-2);

        /* color swatch dot */
        &[data-color-key]::before {
            content: "";
            display: inline-block;
            flex-shrink: 0;
            width: 0.5em;
            height: 0.5em;
            border-radius: 50%;
            background: var(--filter-item-color, var(--color));
            opacity: 0.7;
            margin-right: 0.3em;
            vertical-align: middle;
            transition: opacity var(--speed);
        }

        &:hover[data-color-key]::before,
        &.active[data-color-key]::before {
            opacity: 1;
        }

        &.active {
            box-shadow: 0 0 16px color-mix(in hsl, var(--filter-item-color, var(--filter-accent, var(--accent-info))) 40%, transparent);
        }
    }

    /* Toggle "show more / less" button */
    .filter-tabs__toggle {
        margin-left: auto;
        flex-shrink: 0;
        font-family: var(--header-font);
        font-size: var(--text-xs);
        letter-spacing: 0.06em;
        text-transform: uppercase;
        font-style: italic;
        padding: var(--space-1) 0;
        border: none;
        background: transparent;
        color: var(--accent-action);
        opacity: 0.5;
        cursor: pointer;
        text-decoration: underline;
        text-underline-offset: 3px;
        text-decoration-color: color-mix(in hsl, var(--accent-action) 40%, transparent);
        transition:
            opacity var(--speed),
            text-decoration-color var(--speed);

        &:hover {
            opacity: 1;
            text-decoration-color: var(--accent-action);
        }
    }

    /* ── Mobile select ── */
    .filter-select-wrap {
        display: none;
        position: relative;
        flex: 1;
        min-width: 0;

        &::after {
            content: "";
            pointer-events: none;
            position: absolute;
            right: var(--space-2);
            top: 50%;
            transform: translateY(-50%);
            width: 10px;
            height: 6px;
            background-color: var(--color);
            -webkit-mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 10 6'%3E%3Cpath d='M0 0l5 6 5-6' fill='none' stroke='white' stroke-width='1.5'/%3E%3C/svg%3E");
            -webkit-mask-size: 10px 6px;
            -webkit-mask-repeat: no-repeat;
            mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 10 6'%3E%3Cpath d='M0 0l5 6 5-6' fill='none' stroke='white' stroke-width='1.5'/%3E%3C/svg%3E");
            mask-size: 10px 6px;
            mask-repeat: no-repeat;
            transition: background-color var(--speed);
        }

        &:has(.filter-select.active)::after {
            background-color: var(--filter-accent, var(--accent-info));
        }
    }

    .filter-select {
        width: 100%;
        font-family: var(--header-font);
        font-size: var(--text-xs);
        letter-spacing: 0.08em;
        text-transform: uppercase;
        padding: var(--space-1) var(--space-5) var(--space-1) var(--space-2);
        background: transparent;
        border: 1px solid color-mix(in hsl, var(--color) 25%, transparent);
        color: var(--color);
        cursor: pointer;
        outline: none;
        appearance: none;
        -webkit-appearance: none;
        transition:
            border-color var(--speed),
            color var(--speed),
            box-shadow var(--speed);

        option {
            background: var(--background);
            color: var(--color);
            text-transform: uppercase;
        }

        &.active {
            border-color: var(--filter-accent, var(--accent-info));
            color: var(--filter-accent, var(--accent-info));
            box-shadow: 0 0 12px color-mix(in hsl, var(--filter-accent, var(--accent-info)) 30%, transparent);
        }

        &:focus {
            border-color: var(--filter-accent, var(--accent-info));
            box-shadow: 0 0 12px color-mix(in hsl, var(--filter-accent, var(--accent-info)) 30%, transparent);
        }
    }

    @media ${mq.sm} {
        .filter-tabs {
            display: none;
        }
        .filter-select-wrap {
            display: flex;
        }
    }
`

export default styles
