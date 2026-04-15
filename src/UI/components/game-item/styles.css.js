import { css } from "/core/UI.js"

export const styles = css`
    :host {
        display: block;
        position: relative;
        background: var(--item-background, transparent);
        border: var(--border);
        border-left: 3px solid var(--item-rarity-color, var(--neon-c));
        overflow: hidden;
        transition: box-shadow var(--speed) ease-in-out;

        &:hover {
            box-shadow:
                0 0 0 1px var(--item-rarity-color, var(--neon-c)),
                0 0 16px color-mix(in hsl, var(--item-rarity-color, var(--neon-c)) 30%, transparent);
        }

        #item-link {
            display: flex;
            align-items: stretch;
            width: 100%;
            height: 100%;
            text-decoration: none;
            color: inherit;
            cursor: pointer;
        }

        .icon {
            flex-shrink: 0;
            width: var(--icon-2xl);
            height: var(--icon-2xl);
            align-self: center;
            display: flex;
            align-items: center;
            justify-content: center;
            background: color-mix(in hsl, var(--item-rarity-color, var(--neon-c)) 8%, transparent);
            border-right: var(--border-width) solid color-mix(in hsl, var(--item-rarity-color, var(--neon-c)) 25%, transparent);
            box-sizing: border-box;

            img {
                width: 100%;
                height: 100%;
                object-fit: contain;
                object-position: center center;
                image-rendering: pixelated;
                padding: var(--space-2);
                box-sizing: border-box;
            }

            .icon__placeholder {
                width: var(--icon-md);
                height: var(--icon-md);
                opacity: 0.3;
            }
        }

        .content {
            flex: 1;
            min-width: 0;
            display: flex;
            flex-direction: column;
            justify-content: center;
            gap: var(--space-1);
            padding: var(--space-2) var(--space-3);
        }

        .name {
            font-family: var(--header-font);
            font-size: var(--text-md);
            font-weight: 700;
            letter-spacing: 0.04em;
            color: var(--item-rarity-color, var(--neon-c));
            text-shadow: 0 0 8px color-mix(in hsl, var(--item-rarity-color, var(--neon-c)) 50%, transparent);
            white-space: nowrap;
            overflow: hidden;
            text-overflow: ellipsis;
        }

        .footer {
            display: flex;
            align-items: center;
        }

        .price {
            font-family: var(--header-font);
            font-size: var(--text-md);
            font-weight: 700;
            color: var(--neon-g);
            text-shadow: var(--glow-g);
            letter-spacing: 0.04em;
        }

        .price--zero {
            opacity: 0.35;
        }

        .badges {
            display: flex;
            flex-direction: row;
            align-items: center;
            justify-content: flex-end;
            gap: var(--space-1);
            flex-wrap: nowrap;
            overflow: hidden;
        }

        .badge {
            font-size: calc(var(--text-xs) * 0.65);
            font-family: var(--header-font);
            letter-spacing: 0.08em;
            text-transform: uppercase;
            padding: var(--border-width) var(--space-1);
            line-height: 1.5;
            white-space: nowrap;
        }

        .badge--rarity {
            background: var(--item-rarity-color, var(--neon-c));
            color: var(--background);
            box-shadow:
                0 0 6px color-mix(in hsl, var(--item-rarity-color, var(--neon-c)) 70%, transparent),
                0 0 14px color-mix(in hsl, var(--item-rarity-color, var(--neon-c)) 35%, transparent);
        }

        .badge--type {
            border: var(--border);
            color: color-mix(in hsl, white 75%, var(--color) 25%);
        }
    }
`

export default styles
