import { css } from "/core/UI.js"
import { bp } from "/UI/css/breakpoints.js"
import form from "/css/elements/form.css.js"
import input from "/css/elements/input.css.js"

export const styles = css`
    ${form}
    ${input}

    :host {
        main {
            flex: 1;
            min-height: 0;
            display: flex;
            flex-direction: column;
            align-items: center;
            padding: var(--space) var(--space-sm);
            gap: var(--space-sm);
        }

        ui-card {
            width: 100%;
            max-width: 480px;
        }

        /* Withdraw card grows to fill remaining vertical space */
        ui-card:last-child {
            flex: 1;
            display: flex;
            flex-direction: column;
        }

        @media (max-width: ${bp.sm}px) {
            main {
                padding: var(--space-sm) 0;
            }
            ui-card {
                max-width: 100%;
            }
        }
    }

    /* ── Wallet card ── */
    #wallets {
        width: 100%;
        min-height: 0;
        --wallet-address-wrap-display: none;
    }

    /* ── Send form ── */
    #form {
        flex: 1;
        display: flex;
        flex-direction: column;
        gap: var(--space-5);
        width: 100%;
        padding: var(--space-4) var(--space-4) 0;
        box-sizing: border-box;

        &[hidden] {
            display: none;
        }

        input {
            outline: none;
            font-family: var(--content-font);
        }
    }

    /* ── Field layout ── */
    .field-wrap,
    .field-label {
        display: flex;
        flex-direction: column;
        gap: var(--space-1);
    }

    .field-label-text {
        font-family: var(--header-font);
        font-size: var(--text-xs);
        letter-spacing: 0.1em;
        text-transform: uppercase;
        color: var(--color);
        opacity: 0.4;
        padding-left: var(--space-1);
    }

    #address-error,
    #amount-error {
        margin: 0;
        font-size: var(--text-xs);
        color: var(--accent-danger);
        padding-left: var(--space-1);
        visibility: hidden;

        &[visible] {
            visibility: visible;
        }
    }

    .amount-label-row {
        display: flex;
        align-items: baseline;
        justify-content: space-between;
        gap: var(--space-2);
    }

    #available-balance {
        font-size: var(--text-xs);
        font-family: var(--content-font);
        color: var(--color);
        opacity: 0;
        transition: opacity 0.2s ease;
        white-space: nowrap;

        &[visible] {
            opacity: 0.55;
        }
    }

    .amount-input-row {
        position: relative;
        display: flex;
        align-items: center;

        input {
            padding-right: 4rem;
            width: 100%;
        }
    }

    #amount-symbol {
        position: absolute;
        right: var(--space);
        font-family: var(--header-font);
        font-size: var(--text-xs);
        letter-spacing: 0.08em;
        text-transform: uppercase;
        color: var(--accent-info);
        text-shadow: var(--glow-info);
        opacity: 0.75;
        pointer-events: none;
        white-space: nowrap;

        &:empty {
            display: none;
        }
    }

    /* ── Disclaimers ── */
    .disclaimers {
        list-style: none;
        margin: 0;
        padding: 0 var(--space-4);
        display: flex;
        flex-direction: column;
        gap: var(--space-3);
        flex: 1;
        justify-content: center;

        li {
            display: flex;
            align-items: flex-start;
            gap: var(--space-2);
            font-family: var(--content-font);
            font-size: var(--text-xs);
            line-height: 1.55;
            color: var(--color);
            opacity: 0.45;

            &:first-child {
                color: var(--accent-warning);
                opacity: 0.8;
            }
        }
    }

    .disclaimer-icon {
        width: 0.85em;
        height: 0.85em;
        min-width: 0.85em;
        margin-top: 0.2em;
        flex-shrink: 0;
        --svg-color: currentColor;
    }

    /* ── Bottom actions: gas estimate + submit button ── */
    .form-actions {
        margin-top: auto;
        display: flex;
        flex-direction: column;
        gap: var(--space-sm);
        padding: var(--space-sm) 0 var(--space);
    }

    #gas {
        font-size: var(--text-xs);
        opacity: 0.6;
        font-family: var(--content-font);
        padding-left: var(--space-1);
        visibility: hidden;

        &[visible] {
            visibility: visible;
        }
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
