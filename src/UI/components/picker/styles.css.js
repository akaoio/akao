import { css } from "/core/UI.js"

export const styles = css`
    :host {
        /* ── Override modal chrome to match swap card language ── */
        ui-modal {
            /* section surface */
            --background: var(--card-bg, var(--background));

            dialog {
                min-width: 18rem;
                max-width: min(22rem, 90vw);
                max-height: 70vh;
            }

            section {
                padding: 0;
                border-color: var(--accent-info-border);
                box-shadow: 0 0 32px var(--accent-info-glow);
                overflow-y: auto;
            }
        }

        /* ── Search input in header slot ── */
        .search-slot {
            display: flex;
            align-items: center;
            width: 100%;
            padding: var(--space-2) var(--space-sm);
            border-bottom: 1px solid color-mix(in hsl, var(--accent-info) 20%, transparent);
        }

        .picker-search {
            width: 100%;
            background: transparent;
            border: none;
            color: var(--color);
            font-family: var(--font);
            font-size: var(--text-sm);
            padding: var(--space-1) var(--space-sm);
            outline: none;
            &::placeholder {
                color: var(--color);
                opacity: 0.3;
                font-family: var(--header-font);
                font-size: var(--text-xs);
                letter-spacing: 0.06em;
                text-transform: uppercase;
            }
        }

        /* ── Row container ── */
        .picker-row {
            display: contents;
            &[hidden] { display: none; }
        }

        input[type="radio"] {
            display: none;
        }

        /* ── Default mode rows ── */
        label {
            display: flex;
            align-items: center;
            gap: var(--space-sm);
            width: 100%;
            color: var(--color);
            cursor: pointer;
            padding: var(--space-2) var(--space);
            border-bottom: 1px solid color-mix(in hsl, var(--color) 6%, transparent);
            transition: background var(--speed), color var(--speed);
            &:last-of-type { border-bottom: none; }
            &:hover {
                background: var(--accent-info-surface);
                color: var(--accent-info);
            }
        }

        /* ── Token mode rows ── */
        label:has(.token-icon) {
            padding: var(--space-2) var(--space);
            gap: var(--space-sm);
            min-height: calc(var(--icon-sm) + var(--space-2) * 2);
        }

        input[type="radio"]:checked + label {
            background: color-mix(in hsl, var(--accent-action) 10%, transparent);
            border-left: 2px solid var(--accent-action);
            box-shadow: inset 0 0 20px color-mix(in hsl, var(--accent-action) 5%, transparent);
            .token-symbol { color: var(--accent-action); }
        }

        .token-icon {
            width: var(--icon-sm);
            height: var(--icon-sm);
            min-width: var(--icon-sm);
            max-width: var(--icon-sm);
            max-height: var(--icon-sm);
            flex-shrink: 0;
            overflow: hidden;
        }

        .token-symbol {
            font-family: var(--header-font);
            font-size: var(--text-xs);
            letter-spacing: 0.08em;
            text-transform: uppercase;
            color: var(--color);
        }
    }
`

export default styles
