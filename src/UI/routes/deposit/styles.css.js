import { css } from "/core/UI.js"

export const styles = css`
    :host {
        main {
            width: 100%;
            max-width: 100%;
            padding-inline: 0;
        }

        ui-card {
            width: 100%;
        }
    }

    /* ── Card body wrapper ── */
    #deposit-body {
        display: flex;
        flex-direction: column;
        align-items: center;
        gap: var(--space-5);
        padding: var(--space-4) var(--space-4) var(--space-6);
        width: 100%;
        min-width: 0;
        box-sizing: border-box;
        align-self: stretch;
    }

    /* ── Selection badges ── */
    #selection {
        display: flex;
        flex-direction: column;
        align-items: center;
        gap: var(--space-3);
        width: 100%;

        &[hidden] {
            display: none;
        }
    }

    #selection-coin {
        display: flex;
        justify-content: center;
    }

    #selection-chain {
        display: flex;
        flex-direction: column;
        align-items: flex-start;
        gap: var(--space-1);
        width: 100%;

        #chain-badge {
            width: 100%;
            border-radius: var(--radius);
            justify-content: flex-start;
        }
    }

    #chain-warning {
        display: flex;
        align-items: flex-start;
        gap: var(--space-2);
        margin: var(--space-1) 0 0;
        padding: var(--space-2) var(--space-3);
        background: var(--accent-warning-surface);
        border: 1px solid var(--accent-warning-border);
        border-radius: var(--radius);
        font-size: var(--text-xs);
        font-family: var(--content-font);
        line-height: 1.5;
        color: var(--accent-warning);
        opacity: 0.85;
        width: 100%;
        box-sizing: border-box;

        &::before {
            content: "⚠";
            flex-shrink: 0;
            font-size: var(--text-sm);
            line-height: 1.4;
            opacity: 0.9;
        }
    }

    .selection-label {
        font-family: var(--header-font);
        font-size: var(--text-xs);
        letter-spacing: 0.1em;
        text-transform: uppercase;
        color: var(--color);
        opacity: 0.4;
        padding-left: var(--space-1);
    }

    .badge {
        display: inline-flex;
        align-items: center;
        gap: var(--space-2);
        padding: var(--space-2) var(--space-3);
        background: color-mix(in hsl, var(--accent-info) 6%, transparent);
        border: 1px solid color-mix(in hsl, var(--accent-info) 22%, transparent);
        border-radius: var(--radius-pill);
        color: var(--accent-info);
        font-family: var(--header-font);
        font-size: var(--text-xs);
        letter-spacing: 0.08em;
        text-transform: uppercase;
        cursor: pointer;
        outline: none;
        box-shadow: 0 0 8px var(--accent-info-glow);
        text-shadow: var(--glow-info);
        transition:
            border-color var(--speed),
            color var(--speed),
            background var(--speed),
            box-shadow var(--speed);

        &:hover {
            border-color: color-mix(in hsl, var(--accent-info) 60%, transparent);
            background: color-mix(in hsl, var(--accent-info) 12%, transparent);
            box-shadow: 0 0 14px color-mix(in hsl, var(--accent-info) 25%, transparent);
        }

        &:focus-visible {
            border-color: var(--accent-info);
            box-shadow: var(--glow-info);
        }

        &:active {
            opacity: 0.7;
        }

        &[data-empty] {
            border-style: dashed;
            opacity: 0.5;
            box-shadow: none;
            text-shadow: none;
        }
    }

    .badge-icon,
    .badge-chevron {
        width: var(--icon-sm);
        height: var(--icon-sm);
        flex-shrink: 0;
    }

    .badge-chevron {
        opacity: 0.45;
        transition: opacity var(--speed), transform var(--speed);
    }

    #coin-badge:hover .badge-chevron {
        opacity: 1;
    }

    #chain-badge .badge-chevron {
        margin-left: auto;
    }

    #chain-badge:hover .badge-chevron {
        opacity: 1;
        transform: translateX(2px);
    }

    /* ── QR section ── */
    #qr-section {
        display: flex;
        flex-direction: column;
        align-items: center;
        gap: var(--space-3);
        width: 100%;

        &[hidden] {
            display: none;
        }
    }

    #scan-label {
        font-family: var(--header-font);
        font-size: var(--text-xs);
        letter-spacing: 0.12em;
        text-transform: uppercase;
        color: var(--accent-action);
        text-shadow: var(--glow-action);
        opacity: 0.75;
        margin: 0;
    }

    /* Branded QR frame with corner accent marks */
    #qr-frame {
        position: relative;
        display: inline-flex;
        align-items: center;
        justify-content: center;
        padding: var(--space-3);

        ui-qr {
            display: block;
            width: min(72vw, 260px);
            height: min(72vw, 260px);
        }
    }

    /* Glowing corner brackets */
    #qr-corner {
        position: absolute;
        inset: 0;
        pointer-events: none;
    }

    .corner {
        position: absolute;
        width: 18px;
        height: 18px;
        border-color: var(--accent-action);
        border-style: solid;
        border-width: 0;
        filter: drop-shadow(0 0 4px color-mix(in hsl, var(--accent-action) 70%, transparent));
    }

    .corner.tl {
        top: 0;
        left: 0;
        border-top-width: 2px;
        border-left-width: 2px;
    }
    .corner.tr {
        top: 0;
        right: 0;
        border-top-width: 2px;
        border-right-width: 2px;
    }
    .corner.bl {
        bottom: 0;
        left: 0;
        border-bottom-width: 2px;
        border-left-width: 2px;
    }
    .corner.br {
        bottom: 0;
        right: 0;
        border-bottom-width: 2px;
        border-right-width: 2px;
    }

    /* ── Address wrap ── */
    #address-wrap {
        display: flex;
        flex-direction: column;
        align-items: center;
        gap: var(--space-2);
        width: 100%;

        &[hidden] {
            display: none;
        }
    }

    /* Address pill container — full area is the copy target */
    #address-box {
        display: flex;
        align-items: center;
        width: 100%;
        padding: var(--space-2) var(--space-3);
        background: color-mix(in hsl, var(--color) 5%, transparent);
        border: 1px solid color-mix(in hsl, var(--color) 12%, transparent);
        border-radius: var(--radius);
        cursor: pointer;
        transition:
            border-color var(--speed),
            background var(--speed);

        &:hover {
            background: color-mix(in hsl, var(--accent-info) 6%, transparent);
            border-color: var(--accent-info-border);
        }

        &:active {
            opacity: 0.8;
        }
    }

    #address {
        flex: 1;
        font-family: monospace;
        font-size: var(--text-md);
        letter-spacing: 0.04em;
        word-break: break-all;
        line-height: 1.6;

        .addr-hi {
            color: var(--accent-info);
            text-shadow: var(--glow-info);
            font-weight: 600;
        }

        .addr-mid {
            color: var(--color);
            opacity: 0.45;
        }
    }

    #copy-btn {
        display: inline-flex;
        align-items: center;
        justify-content: center;
        flex-shrink: 0;
        opacity: 0.4;
        transition:
            opacity var(--speed),
            filter var(--speed);

        ui-icon {
            --background: transparent;
            --background-inverted: transparent;
            --color: var(--accent-action);
            --color-inverted: var(--accent-action);
            --icon: var(--icon-md);
        }

        &.copied {
            opacity: 1;
            filter: drop-shadow(0 0 4px color-mix(in hsl, var(--accent-action) 60%, transparent));
        }
    }

    #address-box:hover #copy-btn {
        opacity: 0.75;
    }

    /* ── Coin + Chain picker modal lists ── */
    #coin-list,
    #chain-list {
        list-style: none;
        padding: 0;
        margin: 0;
        min-width: 14rem;

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

            ui-svg {
                width: var(--icon-sm);
                height: var(--icon-sm);
                flex-shrink: 0;
            }

            .chain-standard {
                margin-left: auto;
                font-size: 0.8em;
                opacity: 0.4;
                letter-spacing: 0.04em;
            }
        }
    }
`

export default styles
