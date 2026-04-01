import { css } from "/core/UI.js"
import { bp } from "/UI/css/breakpoints.js"

export const styles = css`
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
            padding-top: calc(var(--space-4) + clamp(5rem, 13vw, 9rem));
            padding-bottom: var(--space-6);
            padding-inline: max(var(--space-2), calc((100vw - var(--max-width, 80rem)) / 2));
            border-bottom: var(--border-width) solid var(--game-text-color, var(--game-primary, var(--color)));
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
            z-index: 20;
            background: color-mix(in hsl, var(--background) 94%, transparent);
            backdrop-filter: blur(16px);
            -webkit-backdrop-filter: blur(16px);
            border-bottom: 1px solid color-mix(in hsl, var(--game-primary, var(--neon-c)) 40%, transparent);
            box-shadow:
                0 1px 0 color-mix(in hsl, var(--game-primary, var(--neon-c)) 25%, transparent),
                0 6px 32px color-mix(in hsl, var(--game-primary, var(--neon-c)) 16%, transparent),
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

        /* ── Filter Group row (label + choices) ── */
        .filter-group {
            display: flex;
            align-items: flex-start;
            gap: var(--space-3);
            padding: var(--space-2) var(--space-3);

            & + .filter-group {
                border-top: 1px solid color-mix(in hsl, var(--color) 8%, transparent);
            }
        }

        .filter-group__label {
            flex-shrink: 0;
            width: 3.5rem;
            padding-top: calc(var(--space-1) + 1px);
            font-family: var(--header-font);
            font-size: var(--text-xs);
            letter-spacing: 0.1em;
            text-transform: uppercase;
            color: var(--color);
            opacity: 0.35;
        }

        /* ── Type Tabs ── */
        .type-tabs {
            flex: 1;
            display: flex;
            flex-wrap: wrap;
            gap: var(--space-1);

            /* hide overflow buttons when not expanded */
            &:not(.expanded) button[data-overflow] {
                display: none;
            }
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

        /* toggle "show more / less" button */
        .type-tabs__toggle {
            margin-left: auto;
            flex-shrink: 0;
            font-family: var(--header-font);
            font-size: var(--text-xs);
            letter-spacing: 0.06em;
            text-transform: uppercase;
            font-style: italic;
            padding: var(--space-1) 0;
            border: none;
            background: transparent;
            color: var(--neon-g);
            opacity: 0.5;
            cursor: pointer;
            text-decoration: underline;
            text-underline-offset: 3px;
            text-decoration-color: color-mix(in hsl, var(--neon-g) 40%, transparent);
            transition:
                opacity var(--speed),
                text-decoration-color var(--speed);

            &:hover {
                opacity: 1;
                text-decoration-color: var(--neon-g);
            }
        }

        /* ── Filter Select (mobile replacement for tabs/pills) ── */
        .filter-select-wrap {
            display: none; /* shown only at ≤767px */
            position: relative;
            flex: 1;
            min-width: 0;

            &::after {
                content: "";
                pointer-events: none;
                position: absolute;
                right: var(--space-2);
                top: 50%;
                transform: translateY(-50%);
                width: 10px;
                height: 6px;
                background-color: var(--color);
                -webkit-mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 10 6'%3E%3Cpath d='M0 0l5 6 5-6' fill='none' stroke='white' stroke-width='1.5'/%3E%3C/svg%3E");
                -webkit-mask-size: 10px 6px;
                -webkit-mask-repeat: no-repeat;
                mask-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 10 6'%3E%3Cpath d='M0 0l5 6 5-6' fill='none' stroke='white' stroke-width='1.5'/%3E%3C/svg%3E");
                mask-size: 10px 6px;
                mask-repeat: no-repeat;
                transition: background-color var(--speed);
            }

            &:has(.filter-select.active)::after {
                background-color: var(--select-accent, var(--neon-c));
            }
        }

        .filter-select {
            width: 100%;
            font-family: var(--header-font);
            font-size: var(--text-xs);
            letter-spacing: 0.08em;
            text-transform: uppercase;
            padding: var(--space-1) var(--space-5) var(--space-1) var(--space-2);
            background: transparent;
            border: 1px solid color-mix(in hsl, var(--color) 25%, transparent);
            color: var(--color);
            cursor: pointer;
            outline: none;
            appearance: none;
            -webkit-appearance: none;
            transition:
                border-color var(--speed),
                color var(--speed),
                box-shadow var(--speed);

            option {
                background: var(--background);
                color: var(--color);
                text-transform: uppercase;
            }

            &.active {
                border-color: var(--select-accent, var(--neon-c));
                color: var(--select-accent, var(--neon-c));
                box-shadow: 0 0 12px color-mix(in hsl, var(--select-accent, var(--neon-c)) 30%, transparent);
            }

            &:focus {
                border-color: var(--neon-c);
                box-shadow: var(--glow-c);
            }
        }

        /* ── Rarity Pills ── */
        .rarity-pills {
            flex: 1;
            display: flex;
            flex-wrap: wrap;
            gap: var(--space-1);
        }

        .rarity-pills button {
            display: inline-flex;
            align-items: center;
            gap: 0.35em;
            font-family: var(--header-font);
            font-size: var(--text-xs);
            letter-spacing: 0.08em;
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
                background var(--speed),
                box-shadow var(--speed);

            /* color swatch dot — only on rarity buttons, not "All" */
            &[data-rarity-key]::before {
                content: "";
                display: inline-block;
                flex-shrink: 0;
                width: 0.5em;
                height: 0.5em;
                border-radius: 50%;
                background: var(--rarity-pill-color, var(--color));
                opacity: 0.7;
                transition: opacity var(--speed);
            }

            &:hover {
                border-color: var(--rarity-pill-color, var(--color-accent));
                color: var(--rarity-pill-color, var(--color-accent));
                background: color-mix(in hsl, var(--rarity-pill-color, var(--color-accent)) 10%, transparent);
                box-shadow: 0 0 12px color-mix(in hsl, var(--rarity-pill-color, var(--color-accent)) 30%, transparent);

                &[data-rarity-key]::before {
                    opacity: 1;
                }
            }

            &.active {
                border-color: var(--rarity-pill-color, var(--color-accent));
                color: var(--rarity-pill-color, var(--color-accent));
                background: color-mix(in hsl, var(--rarity-pill-color, var(--color-accent)) 12%, transparent);
                box-shadow: 0 0 16px color-mix(in hsl, var(--rarity-pill-color, var(--color-accent)) 40%, transparent);

                &[data-rarity-key]::before {
                    opacity: 1;
                }
            }
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
            color: var(--color);
            opacity: 0.4;
        }

        .catalog-count {
            flex-shrink: 0;
            font-family: var(--header-font);
            font-size: var(--text-xs);
            letter-spacing: 0.08em;
            text-transform: uppercase;
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
            z-index: 50;
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
            font-family: var(--header-font);
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
        .catalog-grid {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: var(--space-3);
            padding: var(--space-4) 0 var(--space-3);
            transition: opacity 0.15s;

            &.is-loading-all {
                opacity: 0.4;
                pointer-events: none;
            }
        }

        /* ── Responsive Breakpoints ── */

        /* md: Tablets (768–1023px) */
        @media (max-width: ${bp.md}px) {
            .game-hero {
                padding-top: calc(var(--space-4) + clamp(4rem, 11vw, 7rem));
                padding-bottom: var(--space-5);
            }
        }

        /* sm: Large phones / small tablets (541–767px) */
        @media (max-width: ${bp.sm}px) {
            .game-hero {
                padding-top: calc(var(--space-4) + clamp(3.5rem, 10vw, 6rem));
                padding-bottom: var(--space-4);
            }

            #toolbar {
                flex-wrap: wrap;
                padding: var(--space-3);
            }

            .sort-bar {
                flex-basis: 100%;
            }

            /* swap buttons → selects */
            .type-tabs,
            .rarity-pills {
                display: none;
            }

            .filter-select-wrap {
                display: flex;
            }
        }

        /* 2-col: narrow tablets / large phones (≤849px) */
        @media (max-width: ${bp.grid2}px) {
            .catalog-grid {
                grid-template-columns: repeat(2, 1fr);
            }
        }

        /* 1-col: phones (≤540px) */
        @media (max-width: ${bp.grid1}px) {
            .catalog-grid {
                grid-template-columns: 1fr;
            }
        }

        /* xs: Mobile phones (≤480px) */
        @media (max-width: ${bp.xs}px) {
            .game-hero {
                padding-top: calc(var(--space-4) + clamp(3rem, 9vw, 4.5rem));
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
                flex: 1;
            }

            .sort-bar {
                order: 3;
                flex-wrap: nowrap;
            }

            .catalog-search-wrap {
                order: 2;
                flex-basis: 100%;
            }

            .filter-group__label {
                width: 3rem;
            }
        }

        /* ── Collapsed pill — tablet + mobile only ── */
        @media (max-width: ${bp.md}px) {
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
                border-color: color-mix(in hsl, var(--game-primary, var(--neon-c)) 60%, transparent);
                box-shadow:
                    0 0 12px color-mix(in hsl, var(--game-primary, var(--neon-c)) 25%, transparent),
                    0 6px 32px color-mix(in hsl, var(--game-primary, var(--neon-c)) 16%, transparent);
            }

            /* ── Pill internals ── */
            .pill__type {
                font-family: var(--header-font);
                font-size: var(--text-xs);
                letter-spacing: 0.08em;
                text-transform: uppercase;
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
                font-family: var(--header-font);
                font-size: var(--text-xs);
                letter-spacing: 0.06em;
                text-transform: uppercase;
                white-space: nowrap;
            }

            .pill__sort {
                font-family: var(--header-font);
                font-size: var(--text-xs);
                letter-spacing: 0.06em;
                text-transform: uppercase;
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
            font-family: var(--header-font);
            font-size: var(--text-xs);
            letter-spacing: 0.12em;
            text-transform: uppercase;
            padding: var(--space-2) var(--space-6);
            border: 1px solid var(--game-text-color, var(--game-primary, var(--neon-c)));
            background: transparent;
            color: var(--game-text-color, var(--game-primary, var(--neon-c)));
            cursor: pointer;
            box-shadow:
                0 0 8px color-mix(in hsl, var(--game-text-color, var(--game-primary, var(--neon-c))) 30%, transparent),
                0 0 24px color-mix(in hsl, var(--game-text-color, var(--game-primary, var(--neon-c))) 12%, transparent);
            transition:
                box-shadow var(--speed),
                background var(--speed);

            &:hover {
                background: color-mix(in hsl, var(--game-text-color, var(--game-primary, var(--neon-c))) 8%, transparent);
                box-shadow:
                    0 0 8px color-mix(in hsl, var(--game-text-color, var(--game-primary, var(--neon-c))) 53%, transparent),
                    0 0 24px color-mix(in hsl, var(--game-text-color, var(--game-primary, var(--neon-c))) 20%, transparent),
                    0 0 48px color-mix(in hsl, var(--game-text-color, var(--game-primary, var(--neon-c))) 10%, transparent);
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
