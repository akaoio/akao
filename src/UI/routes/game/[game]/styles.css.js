import { css } from "/core/UI.js"

export const styles = css`
    :host {
        /* ── Game Hero ── */
        .game-hero {
            padding: var(--space-7) 0 var(--space-5);
            border-bottom: 1px solid var(--neon-c);
            display: flex;
            flex-direction: column;
            gap: var(--space-2);

            .game-hero__eyebrow {
                display: flex;
                flex-wrap: wrap;
                gap: var(--space-1);
                margin-bottom: var(--space-1);
            }

            .game-hero__pill {
                font-size: var(--text-xs);
                font-family: var(--header-font);
                letter-spacing: 0.1em;
                text-transform: uppercase;
                padding: 2px var(--space-2);
                border: 1px solid var(--neon-g);
                color: var(--neon-g);
                text-shadow: var(--glow-g);
            }

            h1 {
                font-family: var(--header-font);
                font-size: clamp(2rem, 5vw, 4rem);
                font-weight: 900;
                letter-spacing: 0.06em;
                line-height: 1.05;
                color: var(--color);
                text-shadow: var(--hero-title-shadow, none);
            }

            p {
                font-size: var(--text-md);
                color: var(--color);
                opacity: 0.6;
                max-width: 56rem;
                line-height: 1.6;
            }

            .game-hero__meta {
                display: flex;
                flex-wrap: wrap;
                gap: var(--space-3);
                margin-top: var(--space-1);
                font-size: var(--text-xs);
                opacity: 0.5;

                strong {
                    color: var(--color-accent);
                    margin-right: var(--space-1);
                }
            }
        }

        /* ── Marketplace Nav (odealo-style tabs) ── */
        .marketplace-nav {
            display: flex;
            flex-direction: column;
            gap: var(--space-2);
            padding: var(--space-3) 0;
            border-bottom: 1px solid var(--border);
        }

        .type-tabs,
        .rarity-pills {
            display: flex;
            flex-wrap: wrap;
            gap: var(--space-1);
        }

        .type-tabs button {
            font-family: var(--header-font);
            font-size: var(--text-xs);
            letter-spacing: 0.08em;
            text-transform: uppercase;
            padding: var(--space-1) var(--space-3);
            border: 1px solid var(--border);
            background: transparent;
            color: var(--color);
            cursor: pointer;
            transition:
                border-color var(--speed),
                color var(--speed),
                box-shadow var(--speed);

            &:hover {
                border-color: var(--neon-c);
                color: var(--neon-c);
            }

            &.active {
                border-color: var(--neon-c);
                color: var(--neon-c);
                box-shadow: var(--glow-c);
                background: color-mix(in hsl, var(--neon-c) 6%, transparent);
            }
        }

        .rarity-pills button {
            font-size: var(--text-xs);
            letter-spacing: 0.06em;
            text-transform: uppercase;
            padding: var(--space-1) var(--space-2);
            border: 1px solid var(--border);
            background: transparent;
            color: var(--color);
            cursor: pointer;
            border-radius: 999px;
            transition:
                border-color var(--speed),
                color var(--speed),
                box-shadow var(--speed);

            &:hover {
                border-color: var(--color-accent);
                color: var(--color-accent);
            }

            &.active {
                border-color: currentColor;
                box-shadow: 0 0 6px currentColor;
            }

            &[data-rarity-key="legendary"] {
                --rarity-pill-color: var(--rarity-legendary);
            }
            &[data-rarity-key="unique"] {
                --rarity-pill-color: var(--rarity-unique);
            }
            &[data-rarity-key="rare"] {
                --rarity-pill-color: var(--rarity-rare);
            }
            &[data-rarity-key="magic"] {
                --rarity-pill-color: var(--rarity-magic);
            }
            &[data-rarity-key="special"] {
                --rarity-pill-color: var(--rarity-special);
            }

            &[data-rarity-key].active,
            &[data-rarity-key]:hover {
                color: var(--rarity-pill-color, var(--color-accent));
                border-color: var(--rarity-pill-color, var(--color-accent));
            }
        }

        /* ── Toolbar ── */
        .catalog-toolbar {
            display: flex;
            align-items: center;
            justify-content: space-between;
            padding: var(--space-2) 0;
            flex-wrap: wrap;
            gap: var(--space-2);
        }

        .catalog-count {
            font-size: var(--text-xs);
            color: var(--color);
            opacity: 0.5;
            letter-spacing: 0.06em;
            text-transform: uppercase;
        }

        .sort-bar {
            display: flex;
            flex-wrap: wrap;
            gap: var(--space-1);
        }

        .sort-bar button {
            font-size: var(--text-xs);
            letter-spacing: 0.06em;
            text-transform: uppercase;
            padding: var(--space-1) var(--space-2);
            border: 1px solid var(--border);
            background: transparent;
            color: var(--color);
            cursor: pointer;
            transition:
                border-color var(--speed),
                color var(--speed);

            &:hover {
                border-color: var(--neon-g);
                color: var(--neon-g);
            }

            &.active {
                border-color: var(--neon-g);
                color: var(--neon-g);
            }
        }

        /* ── Item grid ── */
        .catalog-grid {
            display: grid;
            grid-template-columns: repeat(auto-fill, minmax(280px, 1fr));
            gap: var(--space-3);
            padding: var(--space-3) 0 var(--space-3);
        }

        /* ── Load More ── */
        .load-more-wrap {
            display: flex;
            justify-content: center;
            padding: var(--space-4) 0 var(--space-6);
        }

        .load-more-btn {
            font-family: var(--header-font);
            font-size: var(--text-xs);
            letter-spacing: 0.12em;
            text-transform: uppercase;
            padding: var(--space-2) var(--space-6);
            border: 1px solid var(--neon-c);
            background: transparent;
            color: var(--neon-c);
            cursor: pointer;
            transition:
                box-shadow var(--speed),
                background var(--speed);

            &:hover {
                box-shadow: var(--glow-c);
                background: color-mix(in hsl, var(--neon-c) 8%, transparent);
            }

            &:disabled {
                opacity: 0.4;
                cursor: default;
                pointer-events: none;
            }
        }
    }
`

export default styles
