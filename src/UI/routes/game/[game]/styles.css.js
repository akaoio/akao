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
        }

        /* ── Marketplace Nav ── */
        .marketplace-nav {
            display: flex;
            flex-direction: column;
            gap: 0;
            padding: var(--space-3) 0 var(--space-2);
            border-bottom: 1px solid var(--border);
        }

        /* ── Filter Group row (label + choices) ── */
        .filter-group {
            display: flex;
            align-items: flex-start;
            gap: var(--space-3);
            padding: var(--space-2) var(--space-4);

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
        .filter-select {
            display: none; /* shown only at ≤767px */
            flex: 1;
            min-width: 0;
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
            /* chevron arrow */
            background-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 10 6'%3E%3Cpath d='M0 0l5 6 5-6' fill='none' stroke='%2300e5ff' stroke-width='1.5'/%3E%3C/svg%3E");
            background-repeat: no-repeat;
            background-size: 10px 6px;
            background-position: right var(--space-2) center;
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
                background-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 10 6'%3E%3Cpath d='M0 0l5 6 5-6' fill='none' stroke='currentColor' stroke-width='1.5'/%3E%3C/svg%3E");
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

        .catalog-count {
            flex-shrink: 0;
            font-family: var(--header-font);
            font-size: var(--text-xs);
            letter-spacing: 0.08em;
            color: var(--color);
            opacity: 0.4;
            text-transform: uppercase;
            white-space: nowrap;
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
            grid-template-columns: repeat(auto-fill, minmax(280px, 1fr));
            gap: var(--space-3);
            padding: var(--space-4) 0 var(--space-3);
        }

        /* ── Responsive Breakpoints ── */

        /* md: Tablets (768–1023px) */
        @media (max-width: 1023px) {
            .game-hero {
                padding: var(--space-6) 0 var(--space-4);
            }

            .catalog-grid {
                grid-template-columns: repeat(auto-fill, minmax(260px, 1fr));
            }
        }

        /* sm: Large phones / small tablets (481–767px) */
        @media (max-width: 767px) {
            .game-hero {
                padding: var(--space-5) 0 var(--space-4);
            }

            #toolbar {
                padding: var(--space-3);
            }

            /* swap buttons → selects */
            .type-tabs,
            .rarity-pills { display: none; }

            .filter-select { display: block; }

            .catalog-grid {
                grid-template-columns: repeat(auto-fill, minmax(240px, 1fr));
            }
        }

        /* xs: Mobile phones (≤480px) */
        @media (max-width: 480px) {
            .game-hero {
                padding: var(--space-4) 0 var(--space-3);
                gap: var(--space-1);

                h1 { letter-spacing: 0.04em; }

                p { font-size: var(--text-sm); }
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
                order: 2;
                flex-wrap: nowrap;
            }

            .catalog-search-wrap {
                order: 3;
                flex-basis: 100%;
            }

            .filter-group__label { width: 3rem; }
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
