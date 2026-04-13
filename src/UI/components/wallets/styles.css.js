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

    #balance {
        padding: 0 var(--space);
        font-size: var(--text-sm);
        color: var(--color);
        opacity: 0.7;
        font-variant-numeric: tabular-nums;
        &:empty { display: none; }
    }

    /* ── Identicons scroll area ── */
    ui-identicons {
        flex: 1;
        min-height: 0;
    }

    /* ── Address footer ── */
    #address {
        padding: var(--space-sm) var(--space);
        background: color-mix(in hsl, var(--color) 3%, transparent);
        font-size: var(--text-xs);
        font-family: monospace;
        color: var(--color);
        opacity: 0.5;
        text-transform: lowercase;
        white-space: nowrap;
        overflow: hidden;
        text-overflow: ellipsis;
        min-height: 2.25rem;
        display: flex;
        align-items: center;
        &:empty { opacity: 0.2; }
    }

    /* ── Selects ── */
    #currencies,
    #chains {
        background: color-mix(in hsl, var(--color) 3%, transparent);
        ui-select {
            --select-width: stretch;
            --select-radius: 0;
            --select-border: none;
            --select-bg: transparent;
            --select-bg-hover: color-mix(in hsl, var(--neon-g) 6%, transparent);
            --select-border-hover: none;
            --select-border-focus: none;
        }
    }

    #currencies:empty,
    #chains:empty {
        display: none;
    }
`

export default styles
