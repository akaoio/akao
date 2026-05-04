import { css } from "/core/UI.js"

export const styles = css`
    :host([hidden]) {
        display: none;
    }

    :host {
        display: flex;
        flex-direction: column;
        width: 100%;
    }

    #currency-row,
    #chain-row {
        display: flex;
        flex-direction: column;
        align-items: flex-start;
        gap: var(--space-1);
        padding: 0 var(--space);
        margin-bottom: var(--space-3);
    }

    #currency-row[hidden] {
        display: none;
    }

    ui-label {
        display: block;
    }

    #currency-label,
    #chain-label {
        font-size: var(--text-2xs);
        opacity: 0.75;
    }

    .trigger {
        position: relative;
        display: flex;
        align-items: center;
        gap: var(--space-2);
        width: 100%;
        padding: var(--space-sm) var(--space);
        background: transparent;
        border: 1px solid var(--accent-info-border);
        box-shadow: 0 0 8px var(--accent-info-glow);
        box-sizing: border-box;
        color: var(--color);
        font-family: var(--header-font);
        font-size: var(--text-xs);
        letter-spacing: 0.08em;
        text-transform: uppercase;
        cursor: pointer;
        outline: none;
        transition:
            color var(--speed),
            background var(--speed),
            border-color var(--speed),
            box-shadow var(--speed);

        &::after {
            content: "";
            pointer-events: none;
            width: 10px;
            height: 6px;
            flex-shrink: 0;
            margin-left: auto;
            background-color: currentColor;
            -webkit-mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 10 6'%3E%3Cpath d='M0 0l5 6 5-6' fill='none' stroke='white' stroke-width='1.5'/%3E%3C/svg%3E");
            mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 10 6'%3E%3Cpath d='M0 0l5 6 5-6' fill='none' stroke='white' stroke-width='1.5'/%3E%3C/svg%3E");
            -webkit-mask-size: 10px 6px;
            mask-size: 10px 6px;
            -webkit-mask-repeat: no-repeat;
            mask-repeat: no-repeat;
            opacity: 0.35;
            transition: opacity var(--speed);
        }

        &:hover {
            color: var(--accent-info);
            background: color-mix(in hsl, var(--accent-info) 5%, transparent);
            border-color: var(--accent-info);
            box-shadow: 0 0 16px var(--accent-info-border);
            &::after { opacity: 1; }
        }

        &:focus-visible {
            color: var(--accent-info);
            background: color-mix(in hsl, var(--accent-info) 5%, transparent);
        }

        &:active {
            background: color-mix(in hsl, var(--accent-info) 8%, transparent);
        }
    }

    .trigger-body {
        flex: 1;
        display: flex;
        align-items: center;
        justify-content: center;
        gap: var(--space-2);
        min-width: 0;
        overflow: hidden;
    }

    .trigger-icon {
        width: var(--icon-sm);
        height: var(--icon-sm);
        min-width: var(--icon-sm);
        flex-shrink: 0;
        align-self: center;
    }

    .trigger-value {
        text-shadow: var(--glow-info);
        min-width: 0;
        overflow: hidden;
        text-overflow: ellipsis;
        white-space: nowrap;
    }

    .trigger-placeholder {
        opacity: 0.45;
    }

    .trigger[data-has-value] .trigger-placeholder { display: none; }
    .trigger:not([data-has-value]) .trigger-icon,
    .trigger:not([data-has-value]) .trigger-value { display: none; }


`

export default styles
