import { css } from "/core/UI.js"

export const styles = css`
    :host {
        display: flex;
        flex-direction: column;
        flex: 1;
    }

    /* ── Toolbar ── */
    #toolbar {
        display: flex;
        align-items: center;
        gap: var(--space-2);
        font-family: var(--header-font);
        font-size: var(--text-xs);
        letter-spacing: 0.07em;
        text-transform: uppercase;
        border-bottom: 1px solid var(--card-toolbar-border);
        background: var(--card-toolbar-bg);
        padding: var(--space-3) var(--space-4);
    }

    .toolbar-item {
        display: inline-flex;
        align-items: center;
        gap: 0.35em;
        white-space: nowrap;
    }

    .toolbar-label {
        color: var(--color);
        opacity: 0.4;
    }

    .toolbar-num {
        color: var(--accent-info);
        text-shadow: var(--glow-info);
    }

    @keyframes dot-pulse {
        0%, 100% { opacity: 1; }
        50%       { opacity: 0.3; }
    }

    .toolbar-dot {
        width: 0.45em;
        height: 0.45em;
        border-radius: 50%;
        background: color-mix(in hsl, var(--color) 30%, transparent);
        flex-shrink: 0;
        transition: background var(--speed), box-shadow var(--speed);

        &[data-active="true"] {
            background: var(--accent-action);
            box-shadow: 0 0 6px color-mix(in hsl, var(--accent-action) 60%, transparent);
        }

        &[data-polling="true"] {
            animation: dot-pulse 1s ease-in-out infinite;
        }

        @media (prefers-reduced-motion: reduce) {
            &[data-polling="true"] { animation: none; }
        }
    }

    .toolbar-divider {
        width: 1px;
        height: 0.75em;
        background: color-mix(in hsl, var(--color) 15%, transparent);
        flex-shrink: 0;
        align-self: center;
    }

    .toolbar-spacer {
        flex: 1;
    }

    #toolbar-pools .toolbar-num {
        color: var(--color);
        opacity: 0.55;
        text-shadow: none;
    }

    /* ── Fields ── */
    #fields {
        padding: var(--space-4) var(--space);
        display: flex;
        flex-direction: column;
        gap: var(--space);
    }

    .token-field {
        background: var(--field-bg);
        padding: var(--space) var(--space) var(--space-7);
        display: flex;
        flex-direction: column;
        gap: var(--space-sm);
        transition: background var(--speed);
        &:focus-within {
            background: var(--field-bg-focus);
        }
    }

    .field-label {
        font-family: var(--header-font);
        font-size: var(--text-xs);
        letter-spacing: 0.1em;
        text-transform: uppercase;
        color: var(--color);
        opacity: 0.5;
    }

    .field-row {
        display: flex;
        align-items: center;
        gap: var(--space-sm);
    }

    #amount-in {
        flex: 1;
        background: transparent;
        border: none;
        outline: none;
        color: var(--color);
        font-size: var(--text-xl);
        font-family: var(--font);
        padding: 0;
        min-width: 0;
        -moz-appearance: textfield;
        &::-webkit-inner-spin-button,
        &::-webkit-outer-spin-button { -webkit-appearance: none; }
        &::placeholder { opacity: 0.25; }
        &:disabled {
            opacity: 0.35;
            cursor: not-allowed;
        }
    }

    .quote-display {
        flex: 1;
        font-size: var(--text-xl);
        color: var(--color);
        opacity: 0.5;
        min-width: 0;
        overflow: hidden;
        text-overflow: ellipsis;
        white-space: nowrap;
    }

    .field-hint {
        font-size: var(--text-xs);
        color: var(--color);
        opacity: 0.45;
        min-height: 1em;
        &:empty { display: none; }
    }

    /* ── Flip ── */
    #flip-row {
        display: flex;
        justify-content: center;
        position: relative;
        z-index: 1;
        margin: calc(var(--space-sm) * -1) 0;
    }

    #flip-btn {
        --background: transparent;
        --background-inverted: transparent;
        --color-inverted: var(--accent-action);
        color: color-mix(in hsl, var(--accent-action) 55%, transparent);
        transition: color var(--speed), transform var(--speed);
        &:hover { color: var(--accent-action); }
        &:active { color: var(--color); transform: scale(0.9); }
    }

    /* ── Token trigger ── */
    #from-token,
    #to-token {
        flex-shrink: 0;
    }

    .token-trigger {
        position: relative;
        display: inline-flex;
        align-items: center;
        justify-content: space-between;
        gap: var(--space-2);
        min-width: 9.5rem;
        padding: var(--space-2) var(--space-4);
        background: transparent;
        border: 1px solid color-mix(in hsl, var(--color) 25%, transparent);
        color: var(--color);
        font-family: var(--header-font);
        font-size: var(--text-xs);
        letter-spacing: 0.08em;
        text-transform: uppercase;
        cursor: pointer;
        outline: none;
        transition: border-color var(--speed), color var(--speed), box-shadow var(--speed);

        &::after {
            content: "";
            pointer-events: none;
            width: 10px;
            height: 6px;
            flex-shrink: 0;
            background-color: currentColor;
            -webkit-mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 10 6'%3E%3Cpath d='M0 0l5 6 5-6' fill='none' stroke='white' stroke-width='1.5'/%3E%3C/svg%3E");
            -webkit-mask-size: 10px 6px;
            -webkit-mask-repeat: no-repeat;
            mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 10 6'%3E%3Cpath d='M0 0l5 6 5-6' fill='none' stroke='white' stroke-width='1.5'/%3E%3C/svg%3E");
            mask-size: 10px 6px;
            mask-repeat: no-repeat;
            opacity: 0.45;
            transition: opacity var(--speed);
        }

        &:hover {
            border-color: var(--accent-info);
            color: var(--accent-info);
            box-shadow: 0 0 12px color-mix(in hsl, var(--accent-info) 30%, transparent);
            &::after { opacity: 1; }
        }
        &:focus-visible {
            border-color: var(--accent-info);
            color: var(--accent-info);
            box-shadow: 0 0 12px color-mix(in hsl, var(--accent-info) 30%, transparent);
            &::after { opacity: 1; }
        }
        &:active { box-shadow: none; }

        .trigger-icon {
            width: var(--icon-sm);
            height: var(--icon-sm);
            min-width: var(--icon-sm);
            flex-shrink: 0;
        }

        .trigger-placeholder,
        .trigger-value {
            white-space: nowrap;
            text-align: left;
        }

        .trigger-placeholder { opacity: 0.45; }

        &:not([data-has-value]) .trigger-icon,
        &:not([data-has-value]) .trigger-value { display: none; }

        &[data-has-value] .trigger-placeholder { display: none; }
    }

    /* ── Error ── */
    #error {
        padding: 0 var(--space);
        color: var(--accent-danger);
        font-size: var(--text-sm);
        min-height: 1em;
        &:empty { display: none; }
    }

    /* ── Slippage ── */
    #slippage-row {
        display: flex;
        align-items: center;
        gap: var(--space-sm);
        padding: var(--space-sm) var(--space);
        background: var(--field-inset-bg);
        border-top: 1px solid var(--field-inset-border);
        width: fit-content;
        align-self: flex-end;
        border-left: 1px solid var(--field-inset-border);
        border-radius: 0 0 0 var(--radius-sm);
    }

    .slippage-label {
        font-family: var(--header-font);
        font-size: var(--text-xs);
        letter-spacing: 0.08em;
        text-transform: uppercase;
        color: var(--color);
        opacity: 0.5;
        flex-shrink: 0;
    }

    .slippage-inputs {
        display: flex;
        align-items: center;
        gap: var(--space-1);
        margin-left: auto;
    }

    .slippage-preset {
        background: none;
        border: var(--border-width) solid color-mix(in hsl, var(--color) 15%, transparent);
        color: color-mix(in hsl, var(--color) 55%, transparent);
        font-family: var(--header-font);
        font-size: var(--text-xs);
        letter-spacing: 0.04em;
        padding: var(--space-1) var(--space-sm);
        cursor: pointer;
        transition: border-color var(--speed), color var(--speed), box-shadow var(--speed);
        &:hover {
            border-color: color-mix(in hsl, var(--accent-action) 40%, transparent);
            color: var(--accent-action);
        }
        &.active {
            border-color: color-mix(in hsl, var(--accent-action) 50%, transparent);
            color: var(--accent-action);
            box-shadow: var(--glow-action);
        }
    }

    .slippage-inputs input {
        width: 44px;
        background: transparent;
        border: var(--border-width) solid color-mix(in hsl, var(--color) 15%, transparent);
        color: var(--color);
        font-family: var(--header-font);
        font-size: var(--text-xs);
        padding: var(--space-1) var(--space-sm);
        text-align: right;
        -moz-appearance: textfield;
        transition: border-color var(--speed), box-shadow var(--speed);
        &::-webkit-inner-spin-button,
        &::-webkit-outer-spin-button { -webkit-appearance: none; }
        &:focus {
            outline: none;
            border-color: var(--accent-info);
            box-shadow: var(--glow-info);
        }
    }

    .slippage-unit {
        font-size: var(--text-xs);
        color: var(--color);
        opacity: 0.4;
    }

    /* ── Actions ── */
    .actions {
        display: flex;
        flex-direction: column;
        gap: var(--space-sm);
        padding: var(--space);
        padding-top: var(--space-sm);
    }

    ui-button.full {
        width: 100%;
    }

    #submit {
        --btn-bg: color-mix(in hsl, var(--accent-action) 12%, transparent);
        --btn-border: var(--border-width) solid color-mix(in hsl, var(--accent-action) 50%, transparent);
        --btn-radius: 0;
        --btn-padding: var(--space-sm) var(--space);
        --btn-color: var(--accent-action);
        --btn-font: var(--header-font);
        --btn-font-size: var(--text-md);
        --btn-font-weight: 600;
        --btn-letter-spacing: 0.12em;
        --btn-text-transform: uppercase;
        --btn-bg-hover: color-mix(in hsl, var(--accent-action) 20%, transparent);
        --btn-color-hover: var(--accent-action);
        --btn-border-color-hover: var(--accent-action);
        --btn-glow-hover: var(--glow-action);
    }
`

export default styles
