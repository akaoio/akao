import { css } from "/core/UI.js"

export const styles = css`
    /* ── Host ── */
    :host([hidden]) {
        display: none;
    }

    :host {
        display: flex;
        flex-direction: column;
        width: 100%;
    }

    /* ── Identicons carousel ────────────────────────────────────────────────────
       Compact vertical padding via the CSS custom property the identicons
       component exposes. --space-4 = 1rem (default is --space-6 = 2rem).
       The selected item scales to 1.2× (3.6rem); 1rem padding absorbs the
       0.3rem overshoot without clipping.
       ────────────────────────────────────────────────────────────────────────── */
    ui-identicons {
        flex: 1;
        min-height: 0;
        --identicons-pad-v: var(--space-4);
        /* Hide the #N number indicator — wallet names replace it */
        --identicons-status-left-display: none;
    }

    /* ── Wallet actions slot ─────────────────────────────────────────────────────
       Fills the slot in the identicons status bar. Label input stretches to fill;
       remove button sits flush at the right end.
       ────────────────────────────────────────────────────────────────────────── */
    #wallet-actions {
        display: inline-flex;
        align-items: center;
        gap: var(--space-2);
        pointer-events: all;
        flex: 1;
        min-width: 0;
    }

    /* ── Wallet label row ────────────────────────────────────────────────────────
       Flex row: input grows to fill. On focus (.editing), a brief glow splash
       signals the user has entered edit mode.
       ────────────────────────────────────────────────────────────────────────── */
    #label-wrap {
        display: inline-flex;
        align-items: center;
        flex: 1;
        min-width: 0;
        gap: var(--space-1);
        border-radius: 2px;
        transition: box-shadow var(--speed);

        &.editing {
            animation: label-edit-splash 0.38s ease-out forwards;
        }
    }

    @keyframes label-edit-splash {
        0%   { box-shadow: none; }
        35%  { box-shadow: 0 0 0 2px color-mix(in hsl, var(--accent-action) 45%, transparent); }
        100% { box-shadow: none; }
    }

    /* ── Wallet label input ──────────────────────────────────────────────────────
       Takes the full available width. blur/Escape reverts; confirm button saves.
       ────────────────────────────────────────────────────────────────────────── */
    #label-input {
        flex: 1;
        min-width: 0;
        font-family: var(--header-font);
        font-size: var(--text-sm);
        letter-spacing: 0.08em;
        text-transform: uppercase;
        color: var(--accent-action);
        background: transparent;
        border: none;
        border-bottom: 1px solid transparent;
        outline: none;
        padding: 0 0 1px;
        caret-color: var(--accent-action);
        pointer-events: all;
        transition:
            border-bottom-color var(--speed),
            box-shadow var(--speed),
            opacity var(--speed);

        &::placeholder {
            color: var(--color);
            opacity: 0.35;
            font-size: var(--text-xs);
            text-shadow: none;
        }

        &:hover {
            border-bottom-color: color-mix(in hsl, var(--accent-action) 30%, transparent);
        }

        &:focus {
            border-bottom-color: var(--accent-action);
            box-shadow: 0 1px 0 0 color-mix(in hsl, var(--accent-action) 40%, transparent);
        }

        &:disabled {
            opacity: 0.3;
            pointer-events: none;
            cursor: default;
        }

        /* Dim while the Zen write is in-flight */
        &.saving {
            border-bottom-color: color-mix(in hsl, var(--accent-action) 40%, transparent);
            opacity: 0.6;
        }

        /* Brief green glow after a successful Zen ack */
        &.saved {
            border-bottom-color: var(--neon-g);
            box-shadow: 0 1px 0 0 color-mix(in hsl, var(--neon-g) 40%, transparent);
        }
    }

    /* ── Label confirm (SAVE) button ─────────────────────────────────────────────
       Lives in #wallet-preview-actions and swaps in at the SWAP button's position
       when the user focuses the label input. mousedown is prevented so it never
       steals focus from the input; click commits the value.
       ────────────────────────────────────────────────────────────────────────── */
    #label-confirm {
        display: inline-flex;
        align-items: center;
        justify-content: flex-end;
        flex-shrink: 0;
        min-width: 2.25rem;
        min-height: 2.25rem;
        padding: 0;
        appearance: none;
        background: none;
        border: none;
        cursor: pointer;
        pointer-events: all;
        color: var(--neon-g);
        outline: none;
        transition: filter var(--speed);

        &[hidden] { display: none; }

        ui-svg {
            width: var(--icon-sm);
            height: var(--icon-sm);
            pointer-events: none;
            --svg-color: currentColor;
        }

        &:hover { filter: drop-shadow(0 0 4px color-mix(in hsl, var(--neon-g) 60%, transparent)); }
        &:active { opacity: 0.5; }
    }

    /* ── Preview action buttons (DELETE + SWITCH) ───────────────────────────────
       Appear only when a different wallet is selected. Hidden via [hidden].
       ────────────────────────────────────────────────────────────────────────── */
    #wallet-preview-actions {
        display: inline-flex;
        align-items: center;
        flex-shrink: 0;
        margin-left: auto;

        &[hidden] { display: none; }
    }

    #wallet-remove,
    #wallet-switch {
        display: inline-flex;
        align-items: center;
        justify-content: center;
        flex-shrink: 0;
        min-width: 2.25rem;
        min-height: 2.25rem;
        padding: 0;
        background: none;
        border: none;
        cursor: pointer;
        pointer-events: all;
        outline: none;
        transition:
            opacity var(--speed),
            filter var(--speed);

        ui-svg {
            width: var(--icon-sm);
            height: var(--icon-sm);
            pointer-events: none;
            --svg-color: currentColor;
        }

        &:active { opacity: 0.5; }
    }

    #wallet-remove {
        color: var(--neon-m);
        &:hover { filter: drop-shadow(0 0 4px color-mix(in hsl, var(--neon-m) 60%, transparent)); }
        &[hidden] { display: none; }
    }

    #wallet-switch {
        color: var(--neon-g);
        justify-content: flex-end;
        &:hover { filter: drop-shadow(0 0 4px color-mix(in hsl, var(--neon-g) 60%, transparent)); }
        &[hidden] { display: none; }
    }

    /* ── Delete confirm modal footer ────────────────────────────────────────────
       Single trash button that matches the modal's default close circle style.
       ────────────────────────────────────────────────────────────────────────── */
    #delete-modal-body {
        font-family: var(--header-font);
        font-size: var(--text-sm);
        letter-spacing: 0.06em;
        text-transform: uppercase;
        color: var(--color);
        opacity: 0.8;
        margin: 0;
        padding: var(--space) var(--space);
    }

    #delete-confirm {
        display: flex;
        position: relative;
        align-items: center;
        justify-content: center;
        width: var(--icon-md);
        height: var(--icon-md);
        padding: 0;
        appearance: none;
        background: var(--background-inverted);
        border: none;
        border-radius: 50%;
        cursor: pointer;
        outline: none;
        color: var(--neon-m);
        transition: background var(--speed), filter var(--speed), opacity var(--speed);

        ui-svg {
            width: 50%;
            height: 50%;
            pointer-events: none;
            --svg-color: currentColor;
        }

        &:hover {
            background: var(--neon-m);
            filter: drop-shadow(0 0 6px color-mix(in hsl, var(--neon-m) 60%, transparent));
            color: var(--background);
        }

        &:active { opacity: 0.6; }
    }

    /* ── Merged wallet info row ─────────────────────────────────────────────────
       Address (flex-grow, truncated) on the left; chain trigger pill on the
       right. Single compact band replaces the two previous token-field sections.
       ────────────────────────────────────────────────────────────────────────── */
    #wallet-row {
        display: flex;
        align-items: center;
        gap: var(--space-2);
        padding: var(--space-sm) var(--space);
        background: var(--field-bg);
        transition: background var(--speed);

        &:focus-within {
            background: var(--field-bg-focus);
        }
    }

    /* ── Wallet address — glowing monospace centrepiece ─────────────────────────
       Cyan glow gives the hex address the "system readout" feel.
       flex: 1 + min-width: 0 enables text-overflow: ellipsis inside flex.
       Click to copy: brief green flash via .copied class (600ms).
       ────────────────────────────────────────────────────────────────────────── */
    #address-wrap {
        display: inline-flex;
        align-items: center;
        gap: var(--space-2);
        flex: 1;
        min-width: 0;
    }

    #wallet-num {
        font-family: monospace;
        font-size: var(--text-md);
        letter-spacing: 0.12em;
        color: var(--accent-info);
        text-shadow: var(--glow-info);
        white-space: nowrap;
        flex-shrink: 0;
        user-select: none;
        opacity: 0.5;
    }

    #address {
        min-width: 0; /* required for text-overflow inside flex */
        font-family: monospace;
        font-size: var(--text-md);
        letter-spacing: 0.12em;
        color: var(--accent-info);
        text-shadow: var(--glow-info);
        white-space: nowrap;
        overflow: hidden;
        text-overflow: ellipsis;
        transition:
            color var(--speed),
            text-shadow var(--speed);

        /* Em-dash placeholder while no wallet is connected */
        &:empty::before {
            content: "—";
            opacity: 0.2;
            color: var(--color);
            text-shadow: none;
        }
    }

    /* ── Copy button ───────────────────────────────────────────────────────────
       Sits between #address and the chain trigger. Invisible until address is
       available; uses the icon system — no hardcoded glyphs.
       ────────────────────────────────────────────────────────────────────────── */
    #copy-btn {
        display: inline-flex;
        align-items: center;
        justify-content: center;
        flex-shrink: 0;
        padding: 0;
        background: none;
        border: none;
        opacity: 0.4;
        cursor: pointer;
        outline: none;
        transition:
            opacity var(--speed),
            filter var(--speed);

        ui-icon {
            pointer-events: none;
            /* Override the icon's internal background and color tokens */
            --background: transparent;
            --background-inverted: transparent;
            --color: var(--accent-action);
            --color-inverted: var(--accent-action);
            --icon: var(--icon-md);
        }

        &:hover {
            opacity: 1;
            filter: drop-shadow(0 0 3px color-mix(in hsl, var(--accent-action) 70%, transparent));
        }

        &:focus-visible {
            opacity: 1;
            filter: drop-shadow(0 0 3px color-mix(in hsl, var(--accent-action) 70%, transparent));
        }

        &:active {
            opacity: 0.6;
        }

        &.copied {
            opacity: 1;
            filter: drop-shadow(0 0 3px color-mix(in hsl, var(--accent-action) 70%, transparent));
        }

        &:disabled {
            opacity: 0;
            pointer-events: none;
        }
    }

    /* ── Chain trigger pill ─────────────────────────────────────────────────────
       Inline pill version of the full-width chain selector from the previous
       design. Anchors to the right side of #wallet-row (flex-shrink: 0).
       Resting border glows cyan at low opacity to signal it is the primary
       selector; hover steps the glow up.
       ────────────────────────────────────────────────────────────────────────── */
    .chain-trigger {
        position: relative;
        display: inline-flex;
        align-items: center;
        gap: var(--space-2);
        flex-shrink: 0;
        padding: var(--space-1) var(--space-2);
        background: transparent;
        /* Resting glow: signals this is the primary selector without being loud */
        border: 1px solid color-mix(in hsl, var(--accent-info) 35%, transparent);
        box-shadow: 0 0 8px color-mix(in hsl, var(--accent-info) 12%, transparent);
        color: var(--color);
        font-family: var(--header-font);
        font-size: var(--text-xs);
        letter-spacing: 0.08em;
        text-transform: uppercase;
        cursor: pointer;
        outline: none;
        transition:
            border-color var(--speed),
            color var(--speed),
            box-shadow var(--speed);

        /* Chevron — same SVG mask as swap-form token-trigger */
        &::after {
            content: "";
            pointer-events: none;
            width: 10px;
            height: 6px;
            flex-shrink: 0;
            margin-left: var(--space-1);
            background-color: currentColor;
            -webkit-mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 10 6'%3E%3Cpath d='M0 0l5 6 5-6' fill='none' stroke='white' stroke-width='1.5'/%3E%3C/svg%3E");
            mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 10 6'%3E%3Cpath d='M0 0l5 6 5-6' fill='none' stroke='white' stroke-width='1.5'/%3E%3C/svg%3E");
            -webkit-mask-size: 10px 6px;
            mask-size: 10px 6px;
            -webkit-mask-repeat: no-repeat;
            mask-repeat: no-repeat;
            opacity: 0.45;
            transition: opacity var(--speed);
        }

        /* Hover: full cyan border + stronger glow */
        &:hover {
            border-color: var(--accent-info);
            color: var(--accent-info);
            box-shadow: 0 0 16px color-mix(in hsl, var(--accent-info) 35%, transparent);
            &::after {
                opacity: 1;
            }
        }

        &:focus-visible {
            border-color: var(--accent-info);
            color: var(--accent-info);
            box-shadow: 0 0 16px color-mix(in hsl, var(--accent-info) 35%, transparent);
        }

        /* Active: collapse back to resting glow */
        &:active {
            box-shadow: 0 0 8px color-mix(in hsl, var(--accent-info) 12%, transparent);
        }

        /* Chain icon — same sizing as swap-form trigger-icon */
        .trigger-icon {
            width: var(--icon-sm);
            height: var(--icon-sm);
            min-width: var(--icon-sm);
            flex-shrink: 0;
        }

        .trigger-placeholder {
            opacity: 0.45;
        }

        /* Hide placeholder once a chain is selected */
        &[data-has-value] .trigger-placeholder {
            display: none;
        }

        /* Hide icon + name until a value is set */
        &:not([data-has-value]) .trigger-icon,
        &:not([data-has-value]) #chain-name {
            display: none;
        }
    }

    /* ── Balance inset strip ────────────────────────────────────────────────────
       Always visible — prevents layout shift while the balance loads.
       CSS ::before on #balance shows -- when the element is empty.
       ────────────────────────────────────────────────────────────────────────── */
    #balance-row {
        display: flex;
        align-items: center;
        gap: var(--space-sm);
        padding: var(--space-sm) var(--space);
        background: var(--field-inset-bg);
        border-top: 1px solid var(--field-inset-border);
    }

    /* Muted uppercase label — matches swap-form .toolbar-label */
    .toolbar-label {
        font-family: var(--header-font);
        font-size: var(--text-xs);
        letter-spacing: 0.07em;
        text-transform: uppercase;
        color: var(--color);
        opacity: 0.4;
        flex-shrink: 0;
    }

    /* Pushes balance value to the right */
    .toolbar-spacer {
        flex: 1;
    }

    /* Cyan numeric value — same role as .toolbar-num in swap-form */
    .toolbar-num {
        color: var(--accent-info);
        font-family: var(--header-font);
        font-size: var(--text-xs);
        letter-spacing: 0.07em;
        text-shadow: var(--glow-info);
        font-variant-numeric: tabular-nums;

        /* -- placeholder while balance hasn't loaded yet */
        &:empty::before {
            content: "—";
            opacity: 0.3;
        }
    }

    /* ── Currency selector (deposit/withdraw routes only) ───────────────────────
       Wrapped in field-bg when populated; ui-select stays borderless via
       CSS custom property overrides.
       ────────────────────────────────────────────────────────────────────────── */
    #currencies {
        &:empty {
            display: none;
        }

        &:not(:empty) {
            background: var(--field-bg);
        }

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

    /* ── Chain picker modal list ────────────────────────────────────────────────
       Flat rows: icon + chain name. Active chain highlighted green;
       hover previews in cyan — consistent with the rest of the UI.
       ────────────────────────────────────────────────────────────────────────── */
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
                background: color-mix(in hsl, var(--accent-info) 8%, transparent);
                color: var(--accent-info);
            }

            /* Currently active chain — green accent with glow */
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
        }
    }
`

export default styles
