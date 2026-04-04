import { css } from "/core/UI.js"
import { bp } from "/UI/css/breakpoints.js"

export const styles = css`
    :host {
        display: block;
        overflow-x: clip;

        /* ── Hero ── */
        .profile-hero {
            --page-pad: max(var(--space-2), calc((100vw - var(--max-width, 80rem)) / 2));
            position: relative;
            overflow: visible;
            box-sizing: border-box;
            width: 100vw;
            margin-left: calc(50% - 50vw);
            margin-top: calc(-1 * var(--space-4));
            /* Height driven by padding — same as game-hero.
               Top padding mirrors game-hero exactly.
               Bottom padding compensates for the text content (h1 + p + meta)
               that the game-hero has but the profile hero does not. */
            padding-top: calc(var(--space-4) + var(--hero-pad-top));
            padding-bottom: calc(var(--space-6) + clamp(5rem, 10vw, 8rem));
            padding-inline: var(--page-pad);
        }

        .profile-hero__gradient {
            position: absolute;
            inset: 0;
            pointer-events: none;
            isolation: isolate;
            --neon-p: hsl(270 100% 65%);
            --grid-v: 4%;

            background-image:
                /* ① Bottom fade — text legibility */
                linear-gradient(
                    to bottom,
                    transparent 0%,
                    transparent 58%,
                    color-mix(in hsl, var(--background) 35%, transparent) 73%,
                    var(--background) 87%
                ),
                /* ② Dark green base — derived from --neon-g hue, blends with nav + page bg */
                radial-gradient(
                    ellipse 130% 90% at 50% 0%,
                    color-mix(in hsl, var(--neon-g) 18%, var(--background)) 0%,
                    color-mix(in hsl, var(--neon-g) 10%, var(--background)) 50%,
                    var(--background) 100%
                );

            /* ── Ceiling perspective grid ── */
            &::before {
                content: "";
                position: absolute;
                left: -10%;
                right: -10%;
                /* Push well above hero so perspective-compressed lines fill the nav bar gap */
                top: -25%;
                height: 93%;
                pointer-events: none;
                background-image:
                    /* Vertical columns: density controlled by --grid-v */
                    repeating-linear-gradient(
                        to right,
                        transparent 0%,
                        transparent calc(var(--grid-v) - 1px),
                        color-mix(in hsl, var(--neon-g) 28%, transparent) calc(var(--grid-v) - 1px),
                        color-mix(in hsl, var(--neon-g) 28%, transparent) var(--grid-v)
                    ),
                    /* Horizontal rows: percentage stops so the 1px line always sits
                       at the tile edge regardless of tile height, enabling fluid scaling */
                    repeating-linear-gradient(
                        to bottom,
                        transparent 0%,
                        transparent calc(100% - 1px),
                        color-mix(in hsl, var(--neon-g) 20%, transparent) calc(100% - 1px),
                        color-mix(in hsl, var(--neon-g) 20%, transparent) 100%
                    );
                /* Tile height uses clamp: tighter on mobile, roomier on desktop */
                background-size: 100% 100%, 100% clamp(24px, 4vw, 52px);
                background-repeat: no-repeat, repeat-y;
                transform: perspective(600px) rotateX(-52deg);
                transform-origin: 50% 100%;
                mix-blend-mode: screen;
                opacity: 0.7;
                mask-image: linear-gradient(
                    to bottom,
                    black 0%,
                    black 55%,
                    rgba(0, 0, 0, 0.35) 82%,
                    transparent 100%
                );
                -webkit-mask-image: linear-gradient(
                    to bottom,
                    black 0%,
                    black 55%,
                    rgba(0, 0, 0, 0.35) 82%,
                    transparent 100%
                );
            }
        }

        /* ── Avatar anchor (overlaps hero bottom edge by 50%) ── */
        .profile-avatar {
            position: absolute;
            bottom: 0;
            left: var(--page-pad);
            transform: translateY(50%);
            z-index: 1;

            ui-identicon {
                display: block;
                width: var(--avatar-size);
                height: var(--avatar-size);
                color: var(--neon-g);
                background: var(--background);
                border: 2px solid color-mix(in hsl, var(--neon-g) 40%, transparent);
                box-shadow: var(--glow-g);
            }
        }

        .profile-avatar__edit {
            display: none;
            position: absolute;
            inset: 0;
            width: 100%;
            height: 100%;
            align-items: center;
            justify-content: center;
            background: color-mix(in hsl, var(--background) 55%, transparent);
            border: none;
            cursor: pointer;
            color: var(--neon-c);
            opacity: 0;
            transition: opacity var(--speed);

            &:hover { opacity: 1; }
            &:active { opacity: 0.7; }
        }

        #profile-avatar-wrap:hover .profile-avatar__edit,
        #profile-avatar-wrap:focus-within .profile-avatar__edit {
            opacity: 0.85;
        }

        /* ── Avatar picker — sits beside the avatar, same vertical overflow ── */
        .profile-avatar-picker {
            /* items = 75% of picker height → selected at scale(1.2) reaches 90%, leaving
               5% breathing room each side at every breakpoint. No vertical padding needed. */
            --icon-lg: calc(var(--avatar-size) * 0.75);
            --identicons-pad-v: 0px;
            display: none;
            position: absolute;
            bottom: 0;
            /* start right after the avatar */
            left: calc(var(--page-pad) + var(--avatar-size) + var(--space-3));
            right: var(--page-pad);
            height: var(--avatar-size);
            transform: translateY(50%);
            z-index: 1;
            align-items: center;
            overflow: hidden;
            border: var(--border-width) solid color-mix(in hsl, var(--neon-c) 25%, transparent);
            box-shadow: var(--glow-c);
            background: color-mix(in hsl, var(--background) 88%, transparent);
            backdrop-filter: blur(var(--blur-md));

            &.is-open { display: flex; }

            ui-avatars {
                flex: 1;
                min-width: 0;
                align-self: stretch;
            }
        }

        .profile-avatar-picker__loader {
            display: none;
            position: absolute;
            inset: 0;
            align-items: center;
            justify-content: center;
            background: color-mix(in hsl, var(--background) 88%, transparent);
            z-index: 1;

            &::after {
                content: "";
                width: 1.25rem;
                height: 1.25rem;
                border-radius: 50%;
                border: 2px solid transparent;
                border-top-color: var(--neon-c);
                border-right-color: color-mix(in hsl, var(--neon-c) 35%, transparent);
                box-shadow: var(--glow-c);
                animation: picker-spin 0.7s linear infinite;
            }
        }

        .is-loading .profile-avatar-picker__loader { display: flex; }

        /* ── Body ── */
        .profile-body {
            display: flex;
            flex-direction: column;
            gap: var(--space-6);
            padding-top: calc(var(--avatar-size) * 0.5 + var(--space-4));
        }

        /* ── Name row ── */
        .profile-name-row {
            display: flex;
            align-items: flex-end;
            justify-content: space-between;
            gap: var(--space-3);
            padding-bottom: var(--space-4);
            border-bottom: var(--border-width) solid color-mix(in hsl, var(--color) 10%, transparent);
        }

        .profile-name-block {
            display: flex;
            align-items: center;
            gap: var(--space-2);
            flex-wrap: wrap;
            min-width: 0;
        }

        .profile-name {
            font-family: var(--header-font);
            font-size: var(--text-xl);
            font-weight: 700;
            color: var(--color);
            letter-spacing: 0.04em;
            min-width: 0;
            overflow: hidden;
            text-overflow: ellipsis;
            white-space: nowrap;
        }

        .profile-name-edit {
            flex-shrink: 0;
            display: inline-flex;
            align-items: center;
            justify-content: center;
            background: none;
            border: none;
            cursor: pointer;
            color: var(--color);
            opacity: 0.35;
            padding: var(--space-1);
            transition:
                opacity var(--speed),
                color var(--speed);

            &:hover {
                opacity: 1;
                color: var(--neon-c);
            }

            &:active {
                opacity: 0.7;
            }
        }

        .profile-name-input {
            display: none;
            font-family: var(--header-font);
            font-size: var(--text-xl);
            font-weight: 700;
            background: transparent;
            border: none;
            border-bottom: 2px solid var(--neon-c);
            color: var(--color);
            outline: none;
            box-shadow: 0 2px 0 color-mix(in hsl, var(--neon-c) 30%, transparent);
            min-width: 0;
            width: 16rem;
            max-width: 100%;
            letter-spacing: 0.04em;
            transition:
                border-color var(--speed),
                box-shadow var(--speed);

            &:focus {
                box-shadow:
                    0 2px 0 color-mix(in hsl, var(--neon-c) 70%, transparent),
                    var(--glow-c);
            }
        }

        .profile-name-save,
        .profile-name-cancel {
            display: none;
            flex-shrink: 0;
            align-items: center;
            justify-content: center;
            background: none;
            border: var(--border-width) solid transparent;
            cursor: pointer;
            padding: var(--space-1);
            transition:
                color var(--speed),
                border-color var(--speed),
                box-shadow var(--speed);

            &:active {
                opacity: 0.7;
            }
        }

        .profile-name-save {
            color: var(--neon-g);

            &:hover {
                border-color: color-mix(in hsl, var(--neon-g) 50%, transparent);
                box-shadow: var(--glow-g);
            }
        }

        .profile-name-cancel {
            color: var(--neon-m);

            &:hover {
                border-color: color-mix(in hsl, var(--neon-m) 50%, transparent);
                box-shadow: var(--glow-m);
            }
        }

        .profile-auth-controls {
            display: flex;
            align-items: center;
            gap: var(--space-2);
            flex-shrink: 0;
        }

        /* ── Bio row ── */
        .profile-bio-row {
            display: none;
            align-items: flex-start;
            gap: var(--space-2);
            padding-bottom: var(--space-3);
            border-bottom: var(--border-width) solid color-mix(in hsl, var(--color) 10%, transparent);
        }

        .profile-bio {
            flex: 1;
            font-family: var(--content-font);
            font-size: var(--text-sm);
            color: var(--color);
            opacity: 0.75;
            line-height: 1.6;
            min-height: 1.4em;
            white-space: pre-wrap;
            word-break: break-word;
            margin: 0;
        }

        .profile-bio-edit {
            flex-shrink: 0;
            display: inline-flex;
            align-items: center;
            justify-content: center;
            background: none;
            border: none;
            cursor: pointer;
            color: var(--color);
            opacity: 0.35;
            padding: var(--space-1);
            transition:
                opacity var(--speed),
                color var(--speed);

            &:hover {
                opacity: 1;
                color: var(--neon-c);
            }
            &:active {
                opacity: 0.7;
            }
        }

        .profile-bio:empty::before {
            content: "Write something about yourself…";
            opacity: 0.3;
            font-style: italic;
        }

        .profile-bio-edit-wrap {
            display: none;
            flex: 1;
            flex-direction: column;
            gap: var(--space-1);
        }

        .profile-bio-count {
            font-family: var(--content-font);
            font-size: var(--text-xs);
            color: var(--color);
            opacity: 0.35;
            text-align: right;
            letter-spacing: 0.04em;
        }

        .profile-bio-input {
            font-family: var(--content-font);
            font-size: var(--text-sm);
            color: var(--color);
            background: transparent;
            border: var(--border-width) solid var(--neon-c);
            outline: none;
            padding: var(--space-2) var(--space-3);
            resize: vertical;
            min-height: 4.5rem;
            line-height: 1.6;
            transition: box-shadow var(--speed);

            &:focus {
                box-shadow: var(--glow-c);
            }
            &::placeholder {
                color: var(--color);
                opacity: 0.3;
            }
        }

        .profile-bio-save,
        .profile-bio-cancel {
            display: none;
            flex-shrink: 0;
            align-items: center;
            justify-content: center;
            background: none;
            border: var(--border-width) solid transparent;
            cursor: pointer;
            padding: var(--space-1);
            transition:
                color var(--speed),
                border-color var(--speed),
                box-shadow var(--speed);

            &:active {
                opacity: 0.7;
            }
        }

        .profile-bio-save {
            color: var(--neon-g);
            &:hover { opacity: 0.7; }
        }

        .profile-bio-cancel {
            color: var(--neon-m);
            &:hover { opacity: 0.7; }
        }

        /* ── Stats row ── */
        .profile-stats-row {
            display: none;
            gap: var(--space-5);
            flex-wrap: wrap;
        }

        .profile-stat {
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: var(--space-1);
            font-family: var(--header-font);
        }

        .profile-stat__num {
            font-size: var(--text-lg);
            font-weight: 700;
            color: var(--color);
            letter-spacing: 0.04em;
        }

        .profile-stat__label {
            font-size: var(--text-xs);
            letter-spacing: 0.1em;
            text-transform: uppercase;
            color: var(--color);
            opacity: 0.5;
        }

        /* ── Action buttons (Follow / Message — placeholder) ── */
        .profile-actions {
            display: flex;
            gap: var(--space-3);
            flex-wrap: wrap;
        }

        .profile-action-btn {
            display: inline-flex;
            align-items: center;
            gap: var(--space-2);
            font-family: var(--header-font);
            font-size: var(--text-xs);
            letter-spacing: 0.08em;
            text-transform: uppercase;
            padding: var(--space-2) var(--space-4);
            background: none;
            cursor: pointer;
            transition:
                border-color var(--speed),
                box-shadow var(--speed);

            &:disabled {
                opacity: 0.3;
                cursor: not-allowed;
                pointer-events: none;
            }
        }

        .profile-action-btn--follow {
            border: var(--border-width) solid color-mix(in hsl, var(--neon-g) 40%, transparent);
            color: var(--neon-g);
            &:hover:not(:disabled) {
                border-color: var(--neon-g);
                box-shadow: var(--glow-g);
            }
        }

        .profile-action-btn--message {
            border: var(--border-width) solid color-mix(in hsl, var(--neon-c) 40%, transparent);
            color: var(--neon-c);
            &:hover:not(:disabled) {
                border-color: var(--neon-c);
                box-shadow: var(--glow-c);
            }
        }

        /* ── Social links ── */
        .profile-links {
            display: flex;
            flex-wrap: wrap;
            gap: var(--space-2);
        }

        .profile-link {
            display: inline-flex;
            align-items: center;
            justify-content: center;
            color: var(--color);
            opacity: 0.6;
            padding: var(--space-1) var(--space-2);
            border: var(--border-width) solid color-mix(in hsl, var(--color) 15%, transparent);
            transition:
                color var(--speed),
                opacity var(--speed),
                border-color var(--speed),
                box-shadow var(--speed);
            cursor: pointer;
            text-decoration: none;

            &:hover {
                opacity: 1;
                color: var(--neon-c);
                border-color: color-mix(in hsl, var(--neon-c) 40%, transparent);
                box-shadow: var(--glow-c);
            }
        }

        .profile-links-edit-form {
            display: none;
            flex-direction: column;
            gap: var(--space-2);
        }

        .profile-links__field {
            display: flex;
            align-items: center;
            gap: var(--space-3);
        }

        .profile-links__label {
            font-family: var(--header-font);
            font-size: var(--text-xs);
            letter-spacing: 0.08em;
            text-transform: uppercase;
            color: var(--color);
            opacity: 0.6;
            width: 6rem;
            flex-shrink: 0;
        }

        .profile-links__input {
            flex: 1;
            background: transparent;
            border: var(--border-width) solid color-mix(in hsl, var(--color) 20%, transparent);
            color: var(--color);
            font-family: var(--content-font);
            font-size: var(--text-sm);
            padding: var(--space-2) var(--space-3);
            outline: none;
            transition:
                border-color var(--speed),
                box-shadow var(--speed);

            &::placeholder {
                color: var(--color);
                opacity: 0.3;
            }
            &:focus {
                border-color: var(--neon-c);
                box-shadow: var(--glow-c);
            }
        }

        .profile-links-save-row {
            display: none;
            gap: var(--space-2);
        }

        .profile-links-edit-btn {
            display: inline-flex;
            align-items: center;
            gap: var(--space-2);
            background: none;
            border: var(--border-width) solid color-mix(in hsl, var(--color) 20%, transparent);
            color: var(--color);
            font-family: var(--header-font);
            font-size: var(--text-xs);
            letter-spacing: 0.08em;
            text-transform: uppercase;
            padding: var(--space-2) var(--space-3);
            cursor: pointer;
            opacity: 0.6;
            transition:
                opacity var(--speed),
                border-color var(--speed);

            &:hover {
                opacity: 1;
                border-color: color-mix(in hsl, var(--neon-c) 40%, transparent);
            }
            &:active {
                opacity: 0.7;
            }
        }

        /* ── Auth gate ── */
        .profile-gate {
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: var(--space-4);
            padding: var(--space-8) var(--space-4);
            text-align: center;
            color: var(--color);
            opacity: 0.6;
            font-family: var(--content-font);
            font-size: var(--text-md);
        }

        /* ── Sections ── */
        #profile-sections {
            display: flex;
            flex-direction: column;
            gap: var(--space-8);
        }

        .profile-section {
            display: flex;
            flex-direction: column;
            gap: var(--space-3);
        }

        .profile-section__heading {
            font-family: var(--header-font);
            font-size: var(--text-xs);
            font-weight: 600;
            letter-spacing: 0.12em;
            text-transform: uppercase;
            color: var(--neon-g);
            text-shadow: var(--glow-g);
            border-bottom: var(--border-width) solid color-mix(in hsl, var(--neon-g) 25%, transparent);
            padding-bottom: var(--space-1);
            margin: 0;
        }

        /* ── Following list ── */
        .profile-following {
            display: flex;
            flex-direction: column;
            gap: var(--space-1);
        }

        .profile-following__item {
            display: flex;
            align-items: center;
            gap: var(--space-3);
            padding: var(--space-2) var(--space-3);
            border: var(--border-width) solid color-mix(in hsl, var(--color) 10%, transparent);
            transition: border-color var(--speed);

            &:hover {
                border-color: color-mix(in hsl, var(--neon-c) 30%, transparent);
            }
        }

        .profile-following__item-name {
            flex: 1;
            font-family: var(--content-font);
            font-size: var(--text-sm);
            color: var(--color);
            overflow: hidden;
            text-overflow: ellipsis;
            white-space: nowrap;
            min-width: 0;
        }

        .profile-following__unfollow {
            flex-shrink: 0;
            background: none;
            border: var(--border-width) solid color-mix(in hsl, var(--neon-m) 30%, transparent);
            color: var(--neon-m);
            font-family: var(--header-font);
            font-size: var(--text-xs);
            letter-spacing: 0.06em;
            text-transform: uppercase;
            padding: var(--space-1) var(--space-2);
            cursor: pointer;
            transition:
                border-color var(--speed),
                box-shadow var(--speed),
                opacity var(--speed);

            &:hover {
                border-color: var(--neon-m);
                box-shadow: var(--glow-m);
            }

            &:active {
                opacity: 0.7;
            }

            &:disabled {
                opacity: 0.3;
                cursor: not-allowed;
                pointer-events: none;
            }
        }

        /* ── Follow form ── */
        .profile-following__add {
            display: flex;
            gap: var(--space-2);
            flex-wrap: wrap;
        }

        .profile-following__add input {
            flex: 1;
            min-width: 8rem;
            background: transparent;
            border: var(--border-width) solid color-mix(in hsl, var(--color) 20%, transparent);
            color: var(--color);
            font-family: var(--content-font);
            font-size: var(--text-sm);
            padding: var(--space-2) var(--space-3);
            outline: none;
            transition:
                border-color var(--speed),
                box-shadow var(--speed);

            &::placeholder {
                color: var(--color);
                opacity: 0.3;
            }

            &:hover {
                border-color: color-mix(in hsl, var(--color) 40%, transparent);
            }

            &:focus {
                border-color: var(--neon-c);
                box-shadow: var(--glow-c);
            }
        }

        .profile-following__add-btn {
            flex-shrink: 0;
            background: none;
            border: var(--border-width) solid color-mix(in hsl, var(--neon-g) 40%, transparent);
            color: var(--neon-g);
            font-family: var(--header-font);
            font-size: var(--text-xs);
            letter-spacing: 0.08em;
            text-transform: uppercase;
            padding: var(--space-2) var(--space-4);
            cursor: pointer;
            transition:
                border-color var(--speed),
                box-shadow var(--speed),
                background var(--speed);

            &:hover {
                border-color: var(--neon-g);
                box-shadow: var(--glow-g);
                background: color-mix(in hsl, var(--neon-g) 6%, transparent);
            }

            &:active {
                opacity: 0.7;
            }
        }

        /* ── Responsive — mirrors game-hero breakpoints ── */
        @media (max-width: ${bp.md}px) {
            .profile-hero {
                padding-bottom: calc(var(--space-5) + clamp(4rem, 9vw, 7rem));
            }
            /* iPad Air (820px) — ~5.5% keeps physical line spacing close to iPad Pro at 4% */
            .profile-hero__gradient { --grid-v: 5.5%; }
        }

        @media (max-width: ${bp.sm}px) {
            .profile-hero {
                padding-bottom: calc(var(--space-4) + clamp(3.5rem, 8vw, 6rem));
            }

            .profile-name-row {
                flex-direction: column;
                align-items: flex-start;
            }

            .profile-auth-controls {
                padding-left: var(--space-2);
            }
            /* Phone landscape — step down again */
            .profile-hero__gradient { --grid-v: 7.5%; }
        }

        @media (max-width: ${bp.xs}px) {
            .profile-hero {
                padding-bottom: calc(var(--space-3) + clamp(3rem, 7vw, 4.5rem));
            }

            .profile-body {
                padding-top: calc(var(--avatar-size) * 0.5 + var(--space-3));
            }
            /* Small phones — sparse grid to avoid clutter */
            .profile-hero__gradient { --grid-v: 10%; }
        }
    }

    @keyframes picker-spin {
        to { transform: rotate(360deg); }
    }
`

export default styles
