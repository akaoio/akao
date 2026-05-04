import { css } from "/core/UI.js"

export const styles = css`
    :host {
        main {
            flex: 1;
            display: flex;
            flex-direction: column;
            width: 100%;
            max-width: 100%;
            padding-inline: 0;
        }

        ui-card {
            flex: 1;
            width: 100%;
        }
    }

    /* ── Card body wrapper ── */
    #deposit-body {
        display: flex;
        flex-direction: column;
        align-items: center;
        justify-content: space-between;
        gap: var(--space-4);
        padding: var(--space);
        width: 100%;
        min-width: 0;
        box-sizing: border-box;
        align-self: stretch;
        flex: 1;
        opacity: 0;
        transition: opacity calc(var(--speed) * 2);

        &.ready {
            opacity: 1;
        }
    }

    /* ── Actions ── */
    #deposit-actions {
        display: flex;
        flex-direction: column;
        width: 100%;
    }

    #save-share {
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
        width: 100%;
        &[disabled] {
            opacity: 0.35;
            cursor: not-allowed;
            pointer-events: none;
            box-shadow: none;
        }
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

    #selection-coin,
    #selection-chain {
        display: flex;
        flex-direction: column;
        align-items: flex-start;
        gap: var(--space-1);
        width: 100%;
    }

    #chain-warning[hidden] {
        display: none;
    }

    #chain-warning {
        display: flex;
        align-items: center;
        gap: var(--space-2);
        margin: 0;
        padding: var(--space-2) var(--space-3) var(--space-2) var(--space-4);
        background: var(--accent-warning-surface);
        border: 1px solid var(--accent-warning-border);
        border-radius: var(--radius);
        font-size: var(--text-xs);
        font-family: var(--content-font);
        line-height: 1.5;
        color: var(--accent-warning-text);
        width: 100%;
        box-sizing: border-box;

        .warn-icon {
            flex-shrink: 0;
            width: 1.1em;
            height: 1.1em;
            color: var(--accent-warning-text);
            opacity: 0.95;
            font-size: var(--text-sm);
        }
    }

    .selection-label {
        font-family: var(--header-font);
        font-size: var(--text-2xs);
        letter-spacing: 0.1em;
        text-transform: uppercase;
        color: var(--color);
        opacity: 0.75;
    }

    .badge {
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
        border-radius: var(--radius);
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

    .badge-body {
        flex: 1;
        display: flex;
        align-items: center;
        justify-content: center;
        gap: var(--space-2);
        min-width: 0;
        overflow: hidden;
    }

    .badge-icon {
        width: var(--icon-sm);
        height: var(--icon-sm);
        min-width: var(--icon-sm);
        flex-shrink: 0;
        align-self: center;
    }

    #coin-name,
    #chain-name {
        text-shadow: var(--glow-info);
        min-width: 0;
        overflow: hidden;
        text-overflow: ellipsis;
        white-space: nowrap;
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
        font-size: var(--text-md);
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
    /* box-shadow used instead of border so border-radius is preserved while left/right edges fade */
    #address-box {
        display: flex;
        align-items: center;
        width: 100%;
        padding: var(--space-2) var(--space-3);
        background: color-mix(in hsl, var(--color) 5%, transparent);
        border: none;
        border-radius: var(--radius);
        box-shadow:
            0 -1px 0 0 color-mix(in hsl, var(--color) 12%, transparent),
            0 1px 0 0 color-mix(in hsl, var(--color) 12%, transparent),
            -1px 0 0 0 color-mix(in hsl, var(--color) 4%, transparent),
            1px 0 0 0 color-mix(in hsl, var(--color) 4%, transparent);
        cursor: pointer;
        transition:
            box-shadow var(--speed),
            background var(--speed);

        &:hover {
            background: color-mix(in hsl, var(--accent-info) 6%, transparent);
            box-shadow:
                0 -1px 0 0 var(--accent-info-border),
                0 1px 0 0 var(--accent-info-border),
                -1px 0 0 0 color-mix(in hsl, var(--accent-info-border) 30%, transparent),
                1px 0 0 0 color-mix(in hsl, var(--accent-info-border) 30%, transparent);
        }

        &:active {
            opacity: 0.8;
        }
    }

    #address {
        flex: 1;
        font-family: monospace;
        font-size: var(--text-md);
        text-align: center;
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
            opacity: 0.75;
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
`

export default styles
