import fieldset from "/UI/css/elements/fieldset.css.js"
import input from "/UI/css/elements/input.css.js"
import radioItem from "/UI/css/elements/radio-item.css.js"
import { css } from "/core/UI.js"

export const styles = css`
    ${fieldset}
    ${input}
    ${radioItem}
    :host {
        --item-rarity-color: var(--color-accent, #888);

        #item {
            display: grid;
            grid-template-columns: 260px 1fr;
            grid-template-areas:
                "breadcrumb breadcrumb"
                "image header"
                "image main"
                "footer footer";
            gap: var(--space-3);

            @media (max-width: 767px) {
                grid-template-columns: 1fr;
                grid-template-areas:
                    "breadcrumb"
                    "image"
                    "header"
                    "main"
                    "footer";
            }
        }

        #breadcrumb {
            display: flex;
            align-items: center;
            gap: var(--space-2);
            font-size: var(--text-sm);
            opacity: 0.7;
            a { color: var(--item-rarity-color); text-decoration: none; }
            a:hover { text-decoration: underline; }
        }

        #image {
            display: flex;
            align-items: flex-start;
            justify-content: center;
            background: color-mix(in hsl, var(--item-rarity-color) 6%, transparent);
            border: 1px solid color-mix(in hsl, var(--item-rarity-color) 20%, transparent);
            padding: var(--space-4);
            img {
                width: 100%;
                max-width: 220px;
                aspect-ratio: 1;
                object-fit: contain;
                image-rendering: pixelated;
            }
        }

        header {
            .badges {
                display: flex;
                gap: var(--space-1);
                flex-wrap: wrap;
                margin-bottom: var(--space-2);
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
                background: var(--item-rarity-color);
                color: var(--background);
            }
            .badge--type {
                border: 1px solid var(--border);
                color: var(--color);
                opacity: 0.7;
            }
            h1 {
                font-family: var(--header-font);
                font-size: var(--text-xl);
                font-weight: 700;
                color: var(--item-rarity-color);
                text-shadow: 0 0 12px color-mix(in hsl, var(--item-rarity-color) 40%, transparent);
                margin: 0;
            }
        }

        main {
            display: flex;
            flex-direction: column;
            gap: var(--space-3);
        }

        #flavor-text {
            font-style: italic;
            opacity: 0.6;
            font-size: var(--text-sm);
            border-left: 2px solid var(--item-rarity-color);
            padding-left: var(--space-3);
            margin: 0;
        }

        #stats {
            dl#stat-block {
                display: grid;
                grid-template-columns: repeat(auto-fill, minmax(200px, 1fr));
                gap: var(--space-1) var(--space-3);
                margin: 0;
                .stat-row {
                    display: contents;
                    dt {
                        font-size: var(--text-xs);
                        opacity: 0.6;
                        text-transform: uppercase;
                        letter-spacing: 0.05em;
                    }
                    dd {
                        font-family: var(--header-font);
                        font-weight: 700;
                        color: var(--item-rarity-color);
                        margin: 0;
                    }
                }
            }
        }

        #slots {
            #loadout-slots {
                display: flex;
                gap: var(--space-2);
                flex-wrap: wrap;
                .slot-chip {
                    font-size: var(--text-xs);
                    font-family: var(--header-font);
                    text-transform: uppercase;
                    letter-spacing: 0.06em;
                    padding: var(--space-1) var(--space-2);
                    border: 1px solid color-mix(in hsl, var(--item-rarity-color) 40%, transparent);
                    color: var(--item-rarity-color);
                    background: color-mix(in hsl, var(--item-rarity-color) 8%, transparent);
                }
            }
        }

        #pricing {
            display: flex;
            gap: var(--space);
            align-items: center;
            flex-direction: row;
        }

        #quantity {
            display: flex;
            gap: var(--space);
        }

        footer {
            display: flex;
            gap: var(--space-3);
            align-items: flex-end;
        }
    }
`

export default styles
