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
            padding-bottom: var(--space-3);
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
                background: color-mix(in hsl, var(--neon-g) 10%, transparent);
                backdrop-filter: blur(4px);
                -webkit-backdrop-filter: blur(4px);
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
                max-width: 42rem;
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
            margin-top: calc(-1 * var(--space-6));
            margin-bottom: -1px;
            pointer-events: none;
        }

        /* ── Sticky band (filters + toolbar) ── */
        .catalog-sticky {
            position: sticky;
            top: calc(var(--header-height) + var(--space) * 2);
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
            border: 1px solid transparent;
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

        .catalog-toolbar {
            display: flex;
            align-items: center;
            flex-wrap: wrap;
            gap: var(--space-3);
            padding: var(--space-3);
            margin: 0;
            background: color-mix(in hsl, var(--color) 3%, transparent);
            transition:
                border-color var(--speed-2),
                background var(--speed-2),
                box-shadow var(--speed-2);

            &:focus-within {
                background: color-mix(in hsl, var(--neon-c) 3%, transparent);
                animation: toolbar-pulse var(--speed-2) ease forwards;
            }

            ui-search {
                flex: 1;
                min-width: 0;
            }
            .sort-bar {
                flex-shrink: 0;
            }
            .catalog-collapse-btn {
                flex-shrink: 0;
                margin-left: auto;
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
            align-items: center;
            justify-content: flex-end;
            flex-wrap: wrap;
            gap: var(--space-3);
        }

        .sort-bar__label {
            color: var(--color);
            opacity: 0.4;
            font-family: var(--header-font);
            font-size: var(--text-2xs);
            text-transform: uppercase;
            letter-spacing: 0.08em;
            white-space: nowrap;
            align-self: center;
            line-height: normal;

            @media (max-width: 429.98px) {
                display: none;
            }

            @media ${mq.mdDown} {
                padding-left: 0;
            }
        }

        .sort-bar button {
            display: inline-flex;
            justify-content: center;
            align-items: center;
            ${labelFont}
            padding: var(--space-1) var(--space-2);
            border: 1px solid var(--border);
            text-shadow: var(--glow-g);
            box-shadow: 0 0 12px color-mix(in hsl, var(--neon-g) 20%, transparent);
            background: color-mix(in hsl, var(--neon-g) 5%, transparent);
            padding: var(--space-1) var(--space-2);
            color: var(--color);
            cursor: pointer;
            transition:
                border-color var(--speed),
                color var(--speed);

            &:hover:not(:disabled) {
                border-color: var(--neon-g);
                color: var(--neon-g);
            }

            &.active {
                border-color: var(--neon-g);
                color: var(--neon-g);
            }

            &:disabled {
                opacity: 0.3;
                cursor: not-allowed;
                border-color: var(--border);
                color: var(--color);
            }
        }

        /* direction indicator — hidden when button is inactive */
        .sort-dir {
            display: none;
            font-size: var(--text-sm);
            .sort-bar button.active & {
                display: inline-block;
            }
        }

        /* ── Item grid ── */
        .catalog-grid-wrap {
            position: relative;

            &.is-loading-all {
                min-height: 80vh;

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
            padding: var(--space-3) 0 var(--space-3);
        }

        ui-loader {
            display: none;
            position: absolute;
            inset: 0;
            --loader-color: var(--color-accent);
            --loader-min-height: 0;
        }

        /* ── Responsive Breakpoints ── */

        /* mdDown: tablet and below (<768px) */
        @media ${mq.mdDown} {
        }

        /* smDown: small phones and below (<576px) */
        @media ${mq.smDown} {
        }

        /* xsDown: extra small phones (<480px) */
        @media ${mq.xsDown} {
            .game-hero {
                gap: var(--space-1);

                h1 {
                    letter-spacing: 0.04em;
                }

                p {
                    font-size: var(--text-sm);
                }
            }

            .filter-group__label {
                width: 3rem;
            }
        }

        /* mdDown: search full-width on row 1, sort + collapse share row 2 */
        @media ${mq.mdDown} {
            .catalog-toolbar {
                padding: var(--space-3);

                ui-search {
                    flex-basis: 100%;
                }
                .sort-bar {
                    flex: 1;
                    justify-content: flex-start;
                }
                .catalog-collapse-btn {
                    margin-left: 0;
                }
            }
        }

        /* ── Collapsed pill — mdDown only (<768px) ── */
        @media ${mq.mdDown} {
            /* Collapsed: pill only — width is set via JS inline style (pixel value)
               so the width → 100% transition has two concrete lengths to interpolate */
            .catalog-sticky.is-stuck:not(.is-expanded) {
                top: calc(var(--header-height) * 2);
                border-radius: 0 0 8px 0;
                cursor: pointer;
                .marketplace-nav,
                .catalog-toolbar {
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

                /* collapse button becomes visible — no layout change needed */

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
                    color: var(--neon-g);
                    transition: color var(--speed);

                    ui-svg {
                        width: 100%;
                        height: 100%;
                        pointer-events: none;
                    }

                    &:hover {
                        color: var(--game-primary, var(--neon-c));
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
                white-space: nowrap;
                flex-shrink: 0;
                border: none;
                padding: var(--space-1) var(--space-2);
                background: transparent;
                color: color-mix(in hsl, var(--color) 50%, transparent);
                cursor: pointer;
                transition:
                    border-color var(--speed),
                    color var(--speed),
                    background var(--speed),
                    box-shadow var(--speed);

                &:hover {
                    border-color: var(--neon-c);
                    color: var(--neon-c);
                }

                &.active {
                    color: var(--filter-accent, var(--accent-info));
                    background: color-mix(in hsl, var(--filter-accent, var(--accent-info)) 6%, transparent);
                    box-shadow: 0 0 12px color-mix(in hsl, var(--filter-accent, var(--accent-info)) 25%, transparent);
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
                cursor: pointer;
                transition:
                    box-shadow var(--speed),
                    opacity var(--speed);

                &:hover {
                    opacity: 0.6;
                }

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
                box-shadow: 0 0 12px color-mix(in hsl, var(--filter-accent, var(--accent-info)) 25%, transparent);
                background: color-mix(in hsl, var(--filter-accent, var(--accent-info)) 6%, transparent);
                padding: var(--space-1);
            }

            .pill__search {
                flex-shrink: 0;
                color: var(--neon-g);
                opacity: 0.6;
                line-height: 0;
                cursor: pointer;
                transition:
                    color var(--speed),
                    opacity var(--speed);
                padding: var(--space-1);
                border: none;
                border-radius: 999px;
                background: color-mix(in hsl, var(--filter-accent, var(--accent-info)) 6%, transparent);
                box-shadow: 0 0 12px color-mix(in hsl, var(--filter-accent, var(--accent-info)) 25%, transparent);

                ui-svg {
                    width: 16px;
                    height: 16px;
                    pointer-events: none;
                    filter: none;
                    transition: filter var(--speed);
                }

                &:hover {
                    color: var(--neon-c);
                    opacity: 0.8;

                    ui-svg {
                        filter: drop-shadow(0 0 4px color-mix(in hsl, var(--neon-c) 60%, transparent));
                    }
                }

                &:active {
                    opacity: 0.5;
                }

                &.active {
                    color: var(--neon-c);
                    opacity: 1;

                    ui-svg {
                        filter: drop-shadow(0 0 4px color-mix(in hsl, var(--neon-c) 80%, transparent)) drop-shadow(0 0 10px color-mix(in hsl, var(--neon-c) 40%, transparent));
                    }
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

        /* ── Empty state ── */
        .catalog-empty {
            display: flex;
            align-items: center;
            justify-content: center;
            min-height: 160px;

            &[hidden] {
                display: none;
            }
            font-family: var(--header-font);
            font-size: var(--text-sm);
            letter-spacing: 0.08em;
            text-transform: uppercase;
            color: var(--color);
            opacity: 0.35;
        }

        /* ── Load More ── */
        .load-more-wrap {
            display: flex;
            justify-content: center;
        }

        .load-more-btn {
            margin: var(--space-3) 0 var(--space-6);
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
