import { css } from "/core/UI.js"

export const styles = css`
    :host {
        display: flex;
        flex-direction: column;
        flex: 1;
    }

    /* ── Fields ── */
    #fields {
        flex: 1;
        min-height: 0;
        padding: var(--space-2) var(--space);
        display: flex;
        flex-direction: column;
        gap: var(--space-sm);
    }

    .token-field {
        flex: 1;
        min-height: 0;
        background: var(--field-bg);
        padding: var(--space-3) var(--space) var(--space-4);
        position: relative;
        display: flex;
        flex-direction: column;
        justify-content: center;
        gap: var(--space-sm);
        transition: background var(--speed);
        &:focus-within {
            background: var(--field-bg-focus);
        }

        ui-label {
            position: absolute;
            top: var(--space-3);
            left: var(--space);
        }
    }

    ui-label {
        --label-opacity: 65%;
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
        &::-webkit-outer-spin-button {
            -webkit-appearance: none;
        }
        &::placeholder {
            opacity: 0.25;
        }
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
        opacity: 0.65;
        min-height: 1em;
        &:empty {
            display: none;
        }
    }

    /* ── Flip ── */
    #flip-row {
        display: flex;
        justify-content: center;
        position: relative;
        z-index: var(--z-base);
        margin: calc(var(--space-sm) * -1) 0;
    }

    #flip-btn {
        --background: transparent;
        --background-inverted: transparent;
        --color-inverted: var(--accent-action);
        color: color-mix(in hsl, var(--accent-action) 55%, transparent);
        transition:
            color var(--speed),
            transform var(--speed);
        &:hover {
            color: var(--accent-action);
        }
        &:active {
            color: var(--color);
            transform: scale(0.9);
        }
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
        min-width: 8rem;
        min-height: 2.25rem;
        padding: var(--space-1) var(--space-3);
        background: transparent;
        border: 1px solid color-mix(in hsl, var(--color) 25%, transparent);
        color: var(--color);
        font-family: var(--header-font);
        font-size: var(--text-sm);
        letter-spacing: 0.08em;
        text-transform: uppercase;
        cursor: pointer;
        outline: none;
        transition:
            border-color var(--speed),
            color var(--speed),
            box-shadow var(--speed);

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
            &::after {
                opacity: 1;
            }
        }
        &:focus-visible {
            border-color: var(--accent-info);
            color: var(--accent-info);
            box-shadow: 0 0 12px color-mix(in hsl, var(--accent-info) 30%, transparent);
            &::after {
                opacity: 1;
            }
        }
        &:active {
            box-shadow: none;
        }

        .trigger-icon {
            width: 1rem;
            height: 1rem;
            min-width: 1rem;
            flex-shrink: 0;
        }

        .trigger-placeholder,
        .trigger-value {
            white-space: nowrap;
            text-align: left;
        }

        .trigger-placeholder {
            opacity: 0.45;
        }

        &:not([data-has-value]) .trigger-icon,
        &:not([data-has-value]) .trigger-value {
            display: none;
        }

        &[data-has-value] .trigger-placeholder {
            display: none;
        }
    }

    /* ── Error ── */
    #error {
        padding: 0 var(--space);
        color: var(--accent-danger);
        font-size: var(--text-sm);
        min-height: 1em;
        &:empty {
            display: none;
        }
    }

    /* ── Slippage ── */
    #slippage-row {
        display: flex;
        align-items: center;
        gap: var(--space-sm);
        padding: var(--space-2) var(--space);
        border-top: 1px solid var(--card-toolbar-border);
    }

    #slippage-row ui-label {
        flex-shrink: 0;
    }

    .slippage-inputs {
        display: flex;
        align-items: center;
        gap: 4px;
        margin-left: auto;
    }

    .slippage-preset {
        background: none;
        border: var(--border-width) solid color-mix(in hsl, var(--color) 15%, transparent);
        color: color-mix(in hsl, var(--color) 55%, transparent);
        font-family: var(--header-font);
        font-size: var(--text-xs);
        letter-spacing: 0.04em;
        padding: 2px var(--space-1);
        cursor: pointer;
        transition:
            border-color var(--speed),
            color var(--speed),
            box-shadow var(--speed);
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
        width: 36px;
        background: transparent;
        border: var(--border-width) solid color-mix(in hsl, var(--color) 25%, transparent);
        color: var(--color);
        cursor: text;
        font-family: var(--header-font);
        font-size: var(--text-xs);
        padding: 2px var(--space-1);
        text-align: right;
        -moz-appearance: textfield;
        transition:
            border-color var(--speed),
            box-shadow var(--speed);
        &::-webkit-inner-spin-button,
        &::-webkit-outer-spin-button {
            -webkit-appearance: none;
        }
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
        --btn-bg: var(--accent-action-surface);
        --btn-border: var(--border-width) solid var(--accent-action-border);
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
        box-shadow: 0 0 10px var(--accent-action-glow);
        &[disabled] {
            opacity: 0.35;
            cursor: not-allowed;
            pointer-events: none;
            box-shadow: none;
        }
    }
`

export default styles
