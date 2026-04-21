import { css } from "/core/UI.js"
import { mq } from "/UI/css/layout/breakpoints.js"
import grid from "/UI/css/layout/grid.css.js"

const gameColor = `var(--game-text-color, var(--game-primary, var(--neon-c)))`
const gamePrimary = `var(--game-primary, var(--neon-c))`
const labelFont = `font-family: var(--header-font); font-size: var(--text-xs); letter-spacing: 0.06em; text-transform: uppercase;`

export const styles = css`
    ${grid}
    :host {
        display: block;
        overflow-x: clip;

        /* ── Game Hero ── */
        .game-hero {
            position: relative;
            overflow: hidden;
            box-sizing: border-box;
            /* Full-bleed: break out of centered layout container */
            width: 100vw;
            margin-left: calc(50% - 50vw);
            /* Flush with top: cancel layout-main top padding */
            margin-top: calc(-1 * var(--space-4));
            /* Inner content alignment: mirrors layout-main's centering formula */
            padding-top: calc(var(--space-4) + var(--hero-pad-top));
            padding-bottom: var(--space-6);
            padding-inline: max(var(--space-2), calc((100vw - var(--max-width, 80rem)) / 2));
            display: flex;
            flex-direction: column;
            gap: var(--space-2);
            background-image: linear-gradient(to bottom, color-mix(in hsl, var(--background) 10%, transparent) 0%, color-mix(in hsl, var(--background) 55%, transparent) 48%, var(--background) 80%), linear-gradient(to bottom, color-mix(in hsl, var(--game-primary, #000) 18%, transparent) 0%, transparent 55%), var(--game-hero-image, none);
            background-size:
                100% 100%,
                100% 100%,
                100% auto;
            background-position:
                0 0,
                0 0,
                center top;
            background-repeat: no-repeat;

            .game-hero__eyebrow {
                display: flex;
                flex-wrap: wrap;
                gap: var(--space-1);
                margin-bottom: var(--space-1);
            }

            .game-hero__pill {
                ${labelFont}
                letter-spacing: 0.1em;
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
                color: var(--game-text-color, var(--game-primary, var(--color)));
                text-shadow: var(--game-title-shadow, var(--hero-title-shadow, none));
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

        /* ── Sticky sentinel ── */
        .sticky-sentinel {
            height: 1px;
            margin-bottom: -1px;
            pointer-events: none;
        }

        /* ── Sticky band (filters + toolbar) ── */
        .catalog-sticky {
            position: sticky;
            top: calc(var(--header-height) * 2);
            z-index: var(--z-sticky);
            background: color-mix(in hsl, var(--background) 94%, transparent);
            backdrop-filter: blur(16px);
            -webkit-backdrop-filter: blur(16px);
            border-bottom: 1px solid color-mix(in hsl, ${gamePrimary} 40%, transparent);
            box-shadow:
                0 1px 0 color-mix(in hsl, ${gamePrimary} 25%, transparent),
                0 6px 32px color-mix(in hsl, ${gamePrimary} 16%, transparent),
                0 20px 60px color-mix(in hsl, var(--background) 55%, transparent);
            transition:
                width var(--speed-3) cubic-bezier(0.22, 1, 0.36, 1),
                border-radius var(--speed-2);
        }

        /* ── Collapsed pill (hidden by default) ── */
        .catalog-pill {
            display: none;
        }

        /* ── Collapse button (hidden by default) ── */
        .catalog-collapse-btn {
            display: none;
        }

        /* ── Marketplace Nav ── */
        .marketplace-nav {
            display: flex;
            flex-direction: column;
            gap: 0;
            padding-block: var(--space-3);
            border: 1px solid transparent;
        }

        /* Size the search component to fill the flex row like the old input did */
        ui-search {
            flex: 1;
            min-width: 0;
        }

        /* ── Toolbar ── */
        @keyframes toolbar-pulse {
            0% {
                box-shadow: none;
            }
            50% {
                box-shadow: 0 0 32px color-mix(in hsl, var(--neon-c) 18%, transparent);
            }
            100% {
                box-shadow:
                    0 0 0 1px color-mix(in hsl, var(--neon-c) 25%, transparent),
                    0 2px 20px color-mix(in hsl, var(--neon-c) 10%, transparent);
            }
        }

        #toolbar {
            display: flex;
            align-items: center;
            gap: var(--space-3);
            padding: var(--space-3) var(--space-4) var(--space-4);
            margin: 0;
            border: 1px solid color-mix(in hsl, var(--color) 12%, transparent);
            background: color-mix(in hsl, var(--color) 3%, transparent);
            transition:
                border-color var(--speed-2),
                background var(--speed-2),
                box-shadow var(--speed-2);

            &:focus-within {
                border-color: color-mix(in hsl, var(--neon-c) 45%, transparent);
                background: color-mix(in hsl, var(--neon-c) 3%, transparent);
                animation: toolbar-pulse var(--speed-2) ease forwards;
            }
        }

        /* ── Shared count number / label styles ── */
        .count__num {
            color: var(--neon-c);
            text-shadow: var(--glow-c);
        }

        .count__label {
            padding-left: var(--space-1);
            color: var(--color);
            opacity: 0.4;
        }

        .catalog-count {
            flex-shrink: 0;
            ${labelFont}
            letter-spacing: 0.08em;
            white-space: nowrap;
            padding-left: var(--space-2);
        }

        /* ── Search wrap + autocomplete ── */
        .catalog-search-wrap {
            position: relative;
            flex: 1;
            min-width: 0;
        }

        .catalog-search {
            width: 100%;
            background: transparent;
            border: 1px solid color-mix(in hsl, var(--color) 18%, transparent);
            color: var(--color);
            font-family: var(--header-font);
            font-size: var(--text-sm);
            letter-spacing: 0.06em;
            padding: var(--space-2) var(--space-3);
            outline: none;
            transition:
                border-color var(--speed),
                box-shadow var(--speed),
                background var(--speed);

            &::placeholder {
                color: var(--color);
                opacity: 0.2;
            }

            &::-webkit-search-cancel-button {
                filter: invert(0.5);
                cursor: pointer;
            }

            &:focus {
                border-color: var(--neon-c);
                background: color-mix(in hsl, var(--neon-c) 4%, transparent);
                box-shadow: inset 0 0 0 1px color-mix(in hsl, var(--neon-c) 30%, transparent);
            }

            &:not(:placeholder-shown) {
                border-color: color-mix(in hsl, var(--neon-c) 55%, transparent);
            }
        }

        /* ── Autocomplete suggestions ── */
        .catalog-search__suggestions {
            position: absolute;
            top: calc(100% + 4px);
            left: 0;
            right: 0;
            z-index: var(--z-sticky);
            list-style: none;
            background: color-mix(in hsl, var(--background) 96%, var(--neon-c));
            border: 1px solid color-mix(in hsl, var(--neon-c) 40%, transparent);
            box-shadow:
                0 8px 32px color-mix(in hsl, var(--background) 60%, transparent),
                0 0 0 1px color-mix(in hsl, var(--neon-c) 10%, transparent);
            max-height: 280px;
            overflow-y: auto;
            opacity: 0;
            transform: translateY(-6px);
            pointer-events: none;
            transition:
                opacity var(--speed),
                transform var(--speed);
            scrollbar-width: thin;
            scrollbar-color: var(--neon-c) transparent;

            &.open {
                opacity: 1;
                transform: translateY(0);
                pointer-events: auto;
            }
        }

        .catalog-search__suggestion {
            display: flex;
            align-items: center;
            justify-content: space-between;
            gap: var(--space-2);
            padding: var(--space-2) var(--space-3);
            cursor: pointer;
            transition: background var(--speed);

            &:not(:last-child) {
                border-bottom: 1px solid color-mix(in hsl, var(--color) 6%, transparent);
            }

            &:hover,
            &.highlighted {
                background: color-mix(in hsl, var(--neon-c) 8%, transparent);
            }
        }

        .suggestion__name {
            font-family: var(--header-font);
            font-size: var(--text-xs);
            letter-spacing: 0.05em;
            color: var(--color);
            white-space: nowrap;
            overflow: hidden;
            text-overflow: ellipsis;
        }

        .suggestion__rarity {
            flex-shrink: 0;
            font-family: var(--header-font);
            font-size: var(--text-xs);
            letter-spacing: 0.06em;
            text-transform: uppercase;
            color: var(--rarity-pill-color, var(--color-accent));
            opacity: 0.8;
        }

        .sort-bar {
            display: flex;
            flex-wrap: wrap;
            gap: var(--space-1);
        }

        .sort-bar button {
            display: inline-flex;
            align-items: center;
            gap: 0.3em;
            ${labelFont}
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

        /* fixed-width direction slot — always occupies space, visible only when active */
        .sort-dir {
            display: inline-block;
            min-width: 3ch;
            text-align: center;
            opacity: 0;
            transition: opacity var(--speed);

            .sort-bar button.active & {
                opacity: 1;
            }
        }

        /* ── Item grid ── */
        .catalog-grid-wrap {
            position: relative;

            &.is-loading-all {
                min-height: 320px;

                .catalog-grid {
                    display: none;
                }

                ui-loader {
                    display: flex;
                }
            }
        }

        .catalog-grid {
            --grid-cols: 3;
            padding: var(--space-4) 0 var(--space-3);
        }

        ui-loader {
            display: none;
            position: absolute;
            inset: 0;
            --loader-color: var(--color-accent);
            --loader-min-height: 0;
        }

        /* ── Responsive Breakpoints ── */

        /* md: Tablets (768–1023px) */
        @media ${mq.mdDown} {
            .game-hero {
                padding-bottom: var(--space-5);
            }
        }

        /* sm: Large phones / small tablets (541–767px) */
        @media ${mq.smDown} {
            .game-hero {
                padding-bottom: var(--space-4);
            }

            #toolbar {
                flex-wrap: wrap;
                padding: var(--space-3);
            }

            .sort-bar {
                flex-basis: 100%;
            }
        }

        /* xs: Mobile phones (≤480px) */
        @media ${mq.xsDown} {
            .game-hero {
                padding-bottom: var(--space-3);
                gap: var(--space-1);

                h1 {
                    letter-spacing: 0.04em;
                }

                p {
                    font-size: var(--text-sm);
                }
            }

            #toolbar {
                flex-wrap: wrap;
                gap: var(--space-2);
                padding: var(--space-3);
            }

            .catalog-count {
                order: 1;
            }

            ui-search {
                order: 2;
            }

            .sort-bar {
                order: 3;
                flex-basis: 100%;
                flex-wrap: nowrap;
            }

            .filter-group__label {
                width: 3rem;
            }
        }

        /* ── Collapsed pill — tablet + mobile only ── */
        @media ${mq.mdDown} {
            /* Collapsed: pill only — width is set via JS inline style (pixel value)
               so the width → 100% transition has two concrete lengths to interpolate */
            .catalog-sticky.is-stuck:not(.is-expanded) {
                border-radius: 0 0 8px 0;
                cursor: pointer;

                .marketplace-nav,
                #toolbar {
                    display: none;
                }

                .catalog-pill {
                    display: flex;
                    align-items: center;
                    gap: var(--space-2);
                    padding: var(--space-3);
                    cursor: pointer;
                    width: fit-content;
                }
            }

            /* Expanded: full bar as overlay */
            .catalog-sticky.is-stuck.is-expanded {
                width: 100%;
                border-radius: 0;
                cursor: default;
                overflow: visible;
                /* position: sticky (inherited from base rule) establishes a
                   containing block for absolutely-positioned descendants per
                   CSS Positioned Layout §3.4 — position: relative not needed */

                .catalog-pill {
                    display: none;
                }

                .catalog-collapse-btn {
                    display: flex;
                    align-items: center;
                    justify-content: center;
                    flex-shrink: 0;
                    padding: 0;
                    width: 16px;
                    height: 16px;
                    border: none;
                    background: transparent;
                    cursor: pointer;

                    &::before {
                        content: "";
                        display: block;
                        width: 100%;
                        height: 100%;
                        background-color: var(--neon-g);
                        -webkit-mask-image: var(--icon-chevron-double);
                        mask-image: var(--icon-chevron-double);
                        -webkit-mask-size: contain;
                        mask-size: contain;
                        -webkit-mask-repeat: no-repeat;
                        mask-repeat: no-repeat;
                        -webkit-mask-position: center;
                        mask-position: center;
                        transition: background-color var(--speed);
                    }

                    &:hover::before {
                        background-color: var(--game-primary, var(--neon-c));
                    }
                }
            }

            /* Pill hover glow */
            .catalog-sticky.is-stuck:not(.is-expanded):hover {
                border-color: color-mix(in hsl, ${gamePrimary} 60%, transparent);
                box-shadow:
                    0 0 12px color-mix(in hsl, ${gamePrimary} 25%, transparent),
                    0 6px 32px color-mix(in hsl, ${gamePrimary} 16%, transparent);
            }

            /* ── Pill internals ── */
            .pill__type {
                ${labelFont}
                letter-spacing: 0.08em;
                color: var(--color);
                opacity: 0.4;
                white-space: nowrap;
                flex-shrink: 0;
                transition: opacity var(--speed);

                &.active {
                    opacity: 1;
                    color: var(--neon-c);
                }
            }

            .pill__rarity-dot {
                flex-shrink: 0;
                width: 8px;
                height: 8px;
                border-radius: 50%;
                background: var(--pill-rarity-color, var(--color));
                box-shadow: none;
                opacity: 0.2;
                transition:
                    box-shadow var(--speed),
                    opacity var(--speed);

                &.active {
                    opacity: 1;
                    box-shadow: 0 0 6px var(--pill-rarity-color, var(--color-accent));
                }
            }

            .pill__count {
                ${labelFont}
                white-space: nowrap;
            }

            .pill__sort {
                ${labelFont}
                color: var(--neon-g);
                text-shadow: var(--glow-g);
                white-space: nowrap;
            }

            .pill__search {
                flex-shrink: 0;
                color: var(--color);
                opacity: 0.4;
                line-height: 0;
                transition:
                    color var(--speed),
                    opacity var(--speed);

                &.active {
                    color: var(--neon-c);
                    opacity: 1;
                }
            }

            .pill__divider {
                flex-shrink: 0;
                width: 1px;
                height: 0.75em;
                background: color-mix(in hsl, var(--color) 20%, transparent);
                align-self: center;
            }
        }

        /* ── Load More ── */
        .load-more-wrap {
            display: flex;
            justify-content: center;
            padding: var(--space-4) 0 var(--space-6);
        }

        .load-more-btn {
            ${labelFont}
            letter-spacing: 0.12em;
            padding: var(--space-2) var(--space-6);
            border: 1px solid ${gameColor};
            background: transparent;
            color: ${gameColor};
            cursor: pointer;
            box-shadow:
                0 0 8px color-mix(in hsl, ${gameColor} 30%, transparent),
                0 0 24px color-mix(in hsl, ${gameColor} 12%, transparent);
            transition:
                box-shadow var(--speed),
                background var(--speed);

            &:hover {
                background: color-mix(in hsl, ${gameColor} 8%, transparent);
                box-shadow:
                    0 0 8px color-mix(in hsl, ${gameColor} 53%, transparent),
                    0 0 24px color-mix(in hsl, ${gameColor} 20%, transparent),
                    0 0 48px color-mix(in hsl, ${gameColor} 10%, transparent);
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
