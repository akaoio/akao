import { css } from "/core/UI.js"
import { bp } from "/UI/css/breakpoints.js"

export const styles = css`
    :host {
        main {
            display: flex;
            flex-direction: column;
            align-items: center;
            padding: var(--space) var(--space-sm);
        }

        /* Card */
        #swap-card {
            width: 100%;
            max-width: 480px;
            background: color-mix(in hsl, var(--background-inverted) 6%, transparent);
            border: var(--border-width) solid color-mix(in hsl, var(--color) 12%, transparent);
            border-radius: var(--radius-sm);
            overflow: hidden;
        }

        /* Tab bar */
        #tab-bar {
            display: flex;
            align-items: center;
            padding: var(--space-sm) var(--space-sm) 0;
            gap: var(--space-1);
            border-bottom: var(--border-width) solid color-mix(in hsl, var(--color) 10%, transparent);
        }

        .tab {
            background: none;
            border: none;
            color: color-mix(in hsl, var(--color) 45%, transparent);
            font-family: var(--header-font);
            font-size: var(--text-xs);
            letter-spacing: 0.08em;
            text-transform: uppercase;
            padding: var(--space-sm) var(--space);
            cursor: pointer;
            position: relative;
            transition: color var(--speed);
            &::after {
                content: '';
                position: absolute;
                bottom: -1px;
                left: 0;
                right: 0;
                height: 2px;
                background: transparent;
                transition: background var(--speed);
            }
            &:hover {
                color: var(--color);
            }
            &.active {
                color: var(--color);
                font-weight: 600;
                &::after {
                    background: var(--neon-c);
                }
            }
        }

        /* Tab content */
        #tab-content-swap,
        #tab-content-limit {
            padding: var(--space);
            display: flex;
            flex-direction: column;
            gap: var(--space-sm);
        }

        /* Token fields */
        .token-field {
            background: color-mix(in hsl, var(--background-inverted) 5%, transparent);
            border: var(--border-width) solid color-mix(in hsl, var(--color) 10%, transparent);
            border-radius: var(--radius-sm);
            padding: var(--space);
            display: flex;
            flex-direction: column;
            gap: var(--space-sm);
            transition: border-color var(--speed);
            &:focus-within {
                border-color: color-mix(in hsl, var(--neon-c) 50%, transparent);
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
            background: none;
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
            opacity: 0.45;
            min-height: 1em;
            &:empty { display: none; }
        }

        /* Flip button row */
        #flip-row {
            display: flex;
            justify-content: center;
            position: relative;
            z-index: 1;
            margin: calc(var(--space-sm) * -1) 0;
        }

        #flip-btn {
            background: var(--background);
            border: var(--border-width) solid color-mix(in hsl, var(--color) 18%, transparent);
            color: color-mix(in hsl, var(--color) 50%, transparent);
            transition: border-color var(--speed), color var(--speed), transform var(--speed);
            &:hover {
                border-color: color-mix(in hsl, var(--neon-c) 60%, transparent);
                color: var(--neon-c);
            }
            &:active {
                transform: scale(0.9);
            }
        }

        /* Slippage */
        #slippage-row {
            display: flex;
            align-items: center;
            gap: var(--space-sm);
            padding: var(--space-sm) var(--space);
            border: var(--border-width) solid color-mix(in hsl, var(--color) 10%, transparent);
            border-radius: var(--radius-sm);
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
            border-radius: var(--radius-sm);
            cursor: pointer;
            transition: border-color var(--speed), color var(--speed), background var(--speed);
            &:hover {
                border-color: color-mix(in hsl, var(--color) 35%, transparent);
                color: var(--color);
            }
            &.active {
                border-color: color-mix(in hsl, var(--neon-c) 50%, transparent);
                color: var(--neon-c);
                background: color-mix(in hsl, var(--neon-c) 8%, transparent);
            }
        }

        .slippage-inputs input {
            width: 44px;
            background: none;
            border: var(--border-width) solid color-mix(in hsl, var(--color) 15%, transparent);
            border-radius: var(--radius-sm);
            color: var(--color);
            font-family: var(--font);
            font-size: var(--text-xs);
            padding: var(--space-1) var(--space-sm);
            text-align: right;
            -moz-appearance: textfield;
            transition: border-color var(--speed);
            &::-webkit-inner-spin-button,
            &::-webkit-outer-spin-button { -webkit-appearance: none; }
            &:focus {
                outline: none;
                border-color: color-mix(in hsl, var(--neon-c) 50%, transparent);
            }
        }

        .slippage-unit {
            font-size: var(--text-xs);
            color: var(--color);
            opacity: 0.4;
        }

        /* Error */
        #error {
            color: var(--neon-m);
            font-size: var(--text-sm);
            min-height: 1em;
            &:empty { display: none; }
        }

        /* Actions */
        .actions {
            display: flex;
            flex-direction: column;
            gap: var(--space-sm);
            margin-top: var(--space-sm);
        }

        ui-button.full {
            width: 100%;
        }

        @media (max-width: ${bp.sm}px) {
            main {
                padding: var(--space-sm) 0;
            }
            #swap-card {
                border-left: none;
                border-right: none;
                border-radius: 0;
                max-width: 100%;
            }
        }
    }
`

export default styles
