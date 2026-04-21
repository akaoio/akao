import { css } from "/core/UI.js"
import { mq } from "/UI/css/breakpoints.js"

export const styles = css`
    /* ── Keyframes at top level (never nest @keyframes inside a selector) ── */

    @keyframes live-pulse {
        0% {
            box-shadow: 0 0 0 0 color-mix(in hsl, var(--neon-g) 70%, transparent);
        }
        70% {
            box-shadow: 0 0 0 8px transparent;
        }
        100% {
            box-shadow: 0 0 0 0 transparent;
        }
    }

    @keyframes shimmer {
        0% {
            background-position: -600px 0;
        }
        100% {
            background-position: 600px 0;
        }
    }

    @keyframes pool-count-tick {
        0% {
            transform: scale(1);
        }
        40% {
            transform: scale(1.15);
            text-shadow:
                0 0 10px var(--neon-c),
                0 0 28px var(--neon-c);
        }
        100% {
            transform: scale(1);
        }
    }

    @keyframes pool-enter {
        from {
            opacity: 0;
            transform: translateY(-6px);
        }
        to {
            opacity: 1;
            transform: translateY(0);
        }
    }

    :host {
        display: block;

        /* ── Column grid contract — cascades into every <ui-pool> shadow root ── */
        --col-pair: minmax(180px, 2fr);
        --col-tvl: 120px;
        --col-rate: minmax(140px, 1.5fr);
        --col-badges: 160px;

        /* ── Page header ── */
        .page-header {
            padding-top: var(--space-lg);
            padding-bottom: var(--space-sm);
            display: flex;
            flex-direction: column;
            gap: var(--space-1);
        }

        .page-header__title-row {
            display: flex;
            align-items: center;
            gap: var(--space);
        }

        .page-header__title {
            font-family: var(--header-font);
            font-size: clamp(1.5rem, 4vw, 2.5rem);
            font-weight: 900;
            letter-spacing: 0.08em;
            text-transform: uppercase;
            color: var(--neon-c);
            text-shadow: var(--hero-title-shadow);
            line-height: 1;
        }

        .page-header__meta {
            font-family: var(--header-font);
            font-size: clamp(1rem, 2.5vw, 1.4rem);
            letter-spacing: 0.1em;
            text-transform: uppercase;
            opacity: 0.55;
            line-height: 1;
        }

        .loading-status {
            opacity: 0.4;
            font-style: italic;
            letter-spacing: 0.06em;

            &[hidden] {
                display: none;
            }
        }

        .pool-count {
            display: inline-block;
            color: var(--neon-c);
            text-shadow: var(--glow-c);
            opacity: 1;
            transform-origin: center;

            &[hidden] {
                display: none;
            }

            &.ticking {
                animation: pool-count-tick 0.35s ease-out both;
            }
        }

        .pool-count__label {
            &[hidden] {
                display: none;
            }
        }

        /* ── Pulsing live dot ── */
        .live-indicator {
            flex-shrink: 0;
            width: 8px;
            height: 8px;
            border-radius: 50%;
            background: var(--neon-g);
            box-shadow: var(--glow-g);
            animation: live-pulse 2s ease-out infinite;

            &[hidden] {
                display: none;
            }
        }

        /* ── Content (hidden until first data) ── */
        .pools-content {
            display: contents;

            &[hidden] {
                display: none;
            }
        }

        /* ── Sticky sentinel ── */
        .sticky-sentinel {
            height: 1px;
            margin-bottom: -1px;
            pointer-events: none;
        }

        /* ── Sticky filter strip ── */
        .filter-strip {
            position: sticky;
            top: calc(var(--header-height, 2rem) * 2);
            z-index: var(--z-sticky);
            display: flex;
            flex-wrap: wrap;
            background: color-mix(in hsl, var(--background) 92%, transparent);
            backdrop-filter: blur(16px);
            -webkit-backdrop-filter: blur(16px);
            box-shadow:
                0 1px 0 color-mix(in hsl, var(--neon-c) 20%, transparent),
                0 4px 24px color-mix(in hsl, var(--neon-c) 10%, transparent);
            transition: box-shadow var(--speed-3);

            &.is-stuck {
                box-shadow:
                    0 1px 0 color-mix(in hsl, var(--neon-c) 30%, transparent),
                    0 8px 40px color-mix(in hsl, var(--neon-c) 18%, transparent),
                    0 20px 60px color-mix(in hsl, var(--background) 55%, transparent);
            }
        }

        .filter-group {
            display: flex;
            align-items: center;
            gap: var(--space-sm);
            padding: var(--space-sm) var(--space);
            flex-wrap: wrap;

            & + .filter-group {
                border-left: var(--border);
            }
        }

        .filter-group__label {
            flex-shrink: 0;
            font-family: var(--header-font);
            font-size: var(--text-xs);
            letter-spacing: 0.12em;
            text-transform: uppercase;
            opacity: 0.35;
            width: 2.8rem;
        }

        .filter-pills {
            display: flex;
            flex-wrap: wrap;
            gap: var(--space-sm);
        }

        /* ── Chain select ── */
        .filter-select-wrap {
            position: relative;
            min-width: 10rem;

            &::after {
                content: "";
                pointer-events: none;
                position: absolute;
                right: var(--space-sm);
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
                background-color: var(--neon-c);
            }
        }

        .filter-select {
            width: 100%;
            font-family: var(--header-font);
            font-size: var(--text-xs);
            letter-spacing: 0.08em;
            text-transform: uppercase;
            padding: var(--space-sm) var(--space-6) var(--space-sm) var(--space-sm);
            background: transparent;
            border: var(--border);
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
                border-color: var(--neon-c);
                color: var(--neon-c);
                box-shadow: 0 0 12px color-mix(in hsl, var(--neon-c) 30%, transparent);
            }

            &:focus {
                border-color: var(--neon-c);
                box-shadow: var(--glow-c);
            }
        }

        /* ── DEX filter pills ── */
        .filter-pill {
            font-family: var(--header-font);
            font-size: var(--text-xs);
            letter-spacing: 0.08em;
            text-transform: uppercase;
            padding: 3px var(--space-sm);
            border: var(--border);
            background: transparent;
            color: var(--color);
            cursor: pointer;
            transition:
                border-color var(--speed),
                color var(--speed),
                box-shadow var(--speed),
                background var(--speed);

            &:hover {
                border-color: var(--neon-c);
                color: var(--neon-c);
                background: color-mix(in hsl, var(--neon-c) 5%, transparent);
            }

            &.active {
                border-color: var(--neon-c);
                color: var(--neon-c);
                background: color-mix(in hsl, var(--neon-c) 8%, transparent);
                box-shadow: var(--glow-c);
            }
        }

        /* ── Table header ── */
        .table-header {
            display: grid;
            grid-template-columns: var(--col-pair) var(--col-tvl) var(--col-rate) var(--col-badges);
            align-items: center;
            padding: var(--space-sm) var(--space);
            border-bottom: var(--border);
            background: color-mix(in hsl, var(--color) 3%, transparent);
        }

        .th {
            font-family: var(--header-font);
            font-size: var(--text-xs);
            letter-spacing: 0.12em;
            text-transform: uppercase;
            opacity: 0.4;
        }

        .th-tvl,
        .th-rate,
        .th-badges {
            text-align: right;
        }

        /* ── Skeleton shimmer ── */
        .skeleton-list {
            display: flex;
            flex-direction: column;

            &[hidden] {
                display: none;
            }
        }

        .skeleton-row {
            height: 72px;
            border-bottom: var(--border);
            background: linear-gradient(90deg, color-mix(in hsl, var(--color) 4%, transparent) 25%, color-mix(in hsl, var(--color) 9%, transparent) 50%, color-mix(in hsl, var(--color) 4%, transparent) 75%);
            background-size: 600px 100%;
            animation: shimmer 1.6s ease-in-out infinite;
        }

        /* ── Pool list container ── */
        #list {
            display: flex;
            flex-direction: column;
        }

        /* ── New pool entering animation ── */
        ui-pool.entering {
            animation: pool-enter var(--speed-3) cubic-bezier(0.22, 1, 0.36, 1) both;
        }

        /* ── Empty state ── */
        #empty {
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: var(--space);
            padding: var(--space-8) var(--space);
            text-align: center;

            &[hidden] {
                display: none;
            }
        }

        .empty-state__icon {
            font-size: 3rem;
            opacity: 0.12;
        }

        .empty-state__text {
            font-family: var(--header-font);
            font-size: var(--text-sm);
            letter-spacing: 0.1em;
            text-transform: uppercase;
            opacity: 0.4;
        }

        /* ── Load More ── */
        .load-more-wrap {
            display: flex;
            justify-content: center;
            padding: var(--space-lg) 0 var(--space-8);
        }

        .load-more-btn {
            font-family: var(--header-font);
            font-size: var(--text-xs);
            letter-spacing: 0.14em;
            text-transform: uppercase;
            padding: var(--space-sm) var(--space-lg);
            border: var(--border-accent);
            background: transparent;
            color: var(--neon-c);
            cursor: pointer;
            box-shadow: var(--glow-c);
            transition:
                box-shadow var(--speed),
                background var(--speed);

            &[hidden] {
                display: none;
            }

            &:hover {
                background: color-mix(in hsl, var(--neon-c) 8%, transparent);
                box-shadow:
                    0 0 12px color-mix(in hsl, var(--neon-c) 50%, transparent),
                    0 0 36px color-mix(in hsl, var(--neon-c) 20%, transparent);
            }

            &:disabled {
                opacity: 0.35;
                cursor: default;
                pointer-events: none;
            }
        }

        /* ── Responsive: md (≤1023px) ── */
        @media ${mq.mdDown} {
            --col-rate: 0px;

            .th-rate {
                display: none;
            }

            .filter-group + .filter-group {
                border-left: none;
                border-top: var(--border);
                width: 100%;
            }
        }

        /* ── Responsive: sm (≤767px) ── */
        @media ${mq.smDown} {
            .table-header {
                display: none;
            }

            .page-header__title {
                font-size: clamp(1.25rem, 6vw, 1.75rem);
            }
        }
    }
`

export default styles
