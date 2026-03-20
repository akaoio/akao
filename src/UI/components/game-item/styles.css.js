import { css } from "/core/UI.js"

export const styles = css`
    :host {
        display: block;
        position: relative;
        background: var(--item-background, transparent);
        border: var(--border);
        border-left: 3px solid var(--item-rarity-color, var(--neon-c));
        padding: var(--space-3);
        transition: box-shadow var(--speed) ease-in-out;
        cursor: pointer;

        &:hover {
            box-shadow:
                0 0 0 1px var(--item-rarity-color, var(--neon-c)),
                0 0 16px color-mix(in hsl, var(--item-rarity-color, var(--neon-c)) 30%, transparent);
        }

        .badges {
            display: flex;
            gap: var(--space-1);
            margin-bottom: var(--space-2);
            flex-wrap: wrap;
        }

        .badge {
            font-size: calc(var(--text-xs) * 0.9);
            font-family: var(--header-font);
            letter-spacing: 0.08em;
            text-transform: uppercase;
            padding: 2px var(--space-2);
            line-height: 1.6;
        }

        .badge--rarity {
            background: var(--item-rarity-color, var(--neon-c));
            color: var(--background);
        }

        .badge--type {
            border: 1px solid var(--border);
            color: var(--color);
            opacity: 0.7;
        }

        .body {
            display: flex;
            gap: var(--space-3);
            align-items: flex-start;
        }

        .icon {
            flex-shrink: 0;
            width: 48px;
            height: 48px;
            display: flex;
            align-items: center;
            justify-content: center;
            background: color-mix(in hsl, var(--item-rarity-color, var(--neon-c)) 8%, transparent);
            border: 1px solid color-mix(in hsl, var(--item-rarity-color, var(--neon-c)) 30%, transparent);

            img {
                width: 40px;
                height: 40px;
                object-fit: contain;
                image-rendering: pixelated;
            }

            .icon__placeholder {
                width: 24px;
                height: 24px;
                opacity: 0.3;
            }
        }

        .info {
            flex: 1;
            min-width: 0;
        }

        .name {
            font-family: var(--header-font);
            font-size: var(--text-sm);
            font-weight: 700;
            letter-spacing: 0.04em;
            color: var(--item-rarity-color, var(--neon-c));
            text-shadow: 0 0 8px color-mix(in hsl, var(--item-rarity-color, var(--neon-c)) 50%, transparent);
            white-space: nowrap;
            overflow: hidden;
            text-overflow: ellipsis;
            margin-bottom: var(--space-1);

            a {
                color: inherit;
                text-decoration: none;
            }
        }

        .footer {
            display: flex;
            align-items: center;
            justify-content: space-between;
            margin-top: var(--space-2);
        }

        .price {
            font-family: var(--header-font);
            font-size: var(--text-sm);
            font-weight: 700;
            color: var(--neon-g);
            text-shadow: var(--glow-g);
            letter-spacing: 0.04em;
        }

        .price--zero {
            opacity: 0.35;
        }

        /* sm/xs: compact list-style layout — badges now live inside .info so cards are single-row */
        @media (max-width: 767px) {
            :host {
                padding: var(--space-2) var(--space-3);
            }

            .body {
                align-items: center;
                gap: var(--space-2);
            }

            .badges {
                flex-wrap: nowrap;
                margin-bottom: var(--space-1);
            }

            .badge {
                font-size: var(--text-xs);
            }

            .name {
                font-size: var(--text-md);
                margin-bottom: 0;
            }

            .footer {
                margin-top: var(--space-1);
            }
        }
    }
`

export default styles
