import { css } from "/core/UI.js"
import { mq } from "/UI/css/layout/breakpoints.js"

export const styles = css`
    :host {
        display: grid;
        grid-template-columns:
            var(--col-pair, minmax(180px, 2fr))
            var(--col-tvl, 120px)
            var(--col-rate, minmax(140px, 1.5fr))
            var(--col-badges, 160px);
        align-items: center;
        min-height: 56px;
        border-bottom: var(--border);
        position: relative;
        transition: background var(--speed);

        &::before {
            content: "";
            position: absolute;
            left: 0;
            top: 0;
            bottom: 0;
            width: 3px;
            background: var(--neon-c);
            box-shadow: var(--glow-c);
            opacity: 0;
            transition: opacity var(--speed);
        }

        &:hover {
            background: color-mix(in hsl, var(--neon-c) 4%, transparent);

            &::before {
                opacity: 1;
            }
        }
    }

    /* ── Column cells ── */
    .col-pair,
    .col-tvl,
    .col-rate,
    .col-badges {
        padding: var(--space-sm) var(--space);
        min-width: 0;
        overflow: hidden;
    }

    .col-tvl,
    .col-rate,
    .col-badges {
        text-align: right;
    }

    /* ── Token pair ── */
    .pair {
        display: flex;
        align-items: center;
        gap: var(--space-sm);
        font-weight: 600;
        min-width: 0;
    }

    .pair-sep {
        opacity: 0.3;
        flex-shrink: 0;
        font-size: 0.8em;
    }

    /* ── TVL ── */
    .tvl {
        font-family: var(--content-font);
        font-size: var(--text-sm);
        font-weight: 600;
        color: var(--neon-g);
        text-shadow: var(--glow-g);
        white-space: nowrap;

        &:is([data-na]) {
            color: var(--color);
            text-shadow: none;
            opacity: 0.25;
            font-weight: 400;
        }
    }

    /* ── Rates ── */
    .rates {
        display: flex;
        flex-direction: column;
        gap: 2px;
        align-items: flex-end;
    }

    .rate {
        font-size: var(--text-xs);
        opacity: 0.5;
        white-space: nowrap;
        overflow: hidden;
        text-overflow: ellipsis;
        max-width: 100%;
    }

    /* ── Badges ── */
    .badges {
        display: flex;
        gap: var(--space-sm);
        flex-wrap: nowrap;
        justify-content: flex-end;
    }

    .badge {
        display: inline-flex;
        align-items: center;
        gap: 4px;
        padding: 2px var(--space-sm);
        border: var(--border);
        background: var(--background-inverted);
        color: var(--color-inverted);
        font-family: var(--header-font);
        font-size: var(--text-xs);
        letter-spacing: 0.06em;
        white-space: nowrap;
        transition: border-color var(--speed);

        &:hover {
            border-color: var(--neon-c);
        }
    }

    .badge ui-svg {
        width: 1.1em;
        height: 1.1em;
        flex-shrink: 0;
    }

    .version {
        opacity: 0.65;
        font-size: 0.85em;
    }

    /* ── Responsive: md (≤1023px) — hide rate column ── */
    @media ${mq.mdDown} {
        .col-rate {
            display: none;
        }
    }

    /* ── Responsive: sm (≤767px) — 3-col card: pair | rate | badges ── */
    @media ${mq.smDown} {
        :host {
            grid-template-columns: 1fr auto auto;
            grid-template-rows: auto auto;
            align-items: center;
        }

        /* Pair: row 1, col 1 */
        .col-pair {
            grid-column: 1;
            grid-row: 1;
            align-self: center;
            padding-bottom: var(--space-sm);
        }

        /* Stack tokens vertically */
        .pair {
            flex-direction: column;
            align-items: flex-start;
            gap: var(--space-1);
        }

        .pair-sep {
            display: none;
        }

        /* Rate: row 1, col 2 — override the md hide */
        .col-rate {
            display: block;
            grid-column: 2;
            grid-row: 1;
            align-self: center;
            padding-bottom: var(--space-sm);
            text-align: right;
        }

        .rates {
            align-items: flex-end;
        }

        .rate {
            font-size: 0.6rem;
            white-space: normal;
            overflow: visible;
            text-overflow: clip;
            word-break: break-word;
        }

        /* Badges: row 1, col 3 */
        .col-badges {
            grid-column: 3;
            grid-row: 1;
            align-self: center;
            padding-bottom: var(--space-sm);
        }

        .badges {
            flex-direction: column;
            align-items: flex-end;
            gap: var(--space-1);
        }

        /* TVL: row 2, spans all 3 cols */
        .col-tvl {
            grid-column: 1 / 4;
            grid-row: 2;
            text-align: left;
            padding-top: 0;
        }

        .tvl {
            font-size: var(--text-xs);
        }
    }

    /* ── Responsive: xs (≤375px) — iPhone SE: drop rate, smaller fonts ── */
    @media (max-width: 375px) {
        :host {
            grid-template-columns: 1fr auto;
        }

        .col-rate {
            display: none;
        }

        .col-badges {
            grid-column: 2;
        }

        .col-tvl {
            grid-column: 1 / 3;
        }

        .pair {
            gap: 2px;
        }

        .rate {
            font-size: var(--text-xs);
        }

        .tvl {
            font-size: 0.65rem;
        }
    }
`

export default styles
