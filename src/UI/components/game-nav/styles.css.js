import { css } from "/core/UI.js"

export const styles = css`
    :host {
        display: contents;
    }

    /* ── Toggle Tab ── */
    .game-nav__toggle {
        position: fixed;
        top: 50%;
        left: 0;
        z-index: 200;
        transform: translateY(-50%);
        writing-mode: vertical-rl;
        text-orientation: mixed;
        rotate: 180deg;
        padding: var(--space-3) var(--space-2);
        background: var(--background);
        border: 1px solid var(--game-text-color, var(--game-primary, var(--neon-c)));
        border-left: none;
        color: var(--game-text-color, var(--game-primary, var(--neon-c)));
        cursor: pointer;
        box-shadow:
            0 0 8px color-mix(in hsl, var(--game-text-color, var(--game-primary, var(--neon-c))) 53%, transparent),
            0 0 24px color-mix(in hsl, var(--game-text-color, var(--game-primary, var(--neon-c))) 20%, transparent);
        transition:
            background var(--speed),
            box-shadow var(--speed),
            color var(--speed);

        &:hover {
            background: color-mix(in hsl, var(--game-text-color, var(--game-primary, var(--neon-c))) 10%, var(--background));
            box-shadow:
                0 0 8px color-mix(in hsl, var(--game-text-color, var(--game-primary, var(--neon-c))) 53%, transparent),
                0 0 24px color-mix(in hsl, var(--game-text-color, var(--game-primary, var(--neon-c))) 20%, transparent),
                4px 0 24px color-mix(in hsl, var(--game-text-color, var(--game-primary, var(--neon-c))) 13%, transparent);
        }
    }

    .game-nav__toggle-label {
        font-family: var(--header-font);
        font-size: var(--text-xs);
        letter-spacing: 0.12em;
        text-transform: uppercase;
    }

    .game-nav__toggle-icon {
        display: none;
    }

    /* ── Mobile: hide fixed toggle (button moves to header) ── */
    @media (max-width: 767px) {
        .game-nav__toggle {
            display: none;
        }

        .game-nav__toggle-label { display: none; }
        .game-nav__toggle-icon { display: block; }
    }

    /* ── Backdrop ── */
    .game-nav__backdrop {
        position: fixed;
        inset: 0;
        z-index: 210;
        background: color-mix(in hsl, var(--background) 60%, transparent);
        backdrop-filter: blur(4px);
        opacity: 0;
        pointer-events: none;
        transition: opacity var(--speed);

        &.open {
            opacity: 1;
            pointer-events: auto;
        }
    }

    /* ── Slide Panel ── */
    .game-nav__panel {
        position: fixed;
        top: 0;
        left: 0;
        bottom: 0;
        z-index: 220;
        width: min(320px, 85vw);
        background: var(--background);
        border-right: 1px solid var(--game-primary, var(--neon-c));
        box-shadow: 4px 0 40px color-mix(in hsl, var(--game-primary, var(--neon-c)) 13%, transparent);
        display: flex;
        flex-direction: column;
        transform: translateX(-100%);
        transition: transform var(--speed-3) cubic-bezier(0.22, 1, 0.36, 1);
        overflow: hidden;

        &.open {
            transform: translateX(0);
        }
    }

    /* ── Panel Header ── */
    .game-nav__header {
        display: flex;
        align-items: center;
        justify-content: space-between;
        padding: var(--space-4) var(--space-3);
        border-bottom: 1px solid color-mix(in hsl, var(--game-primary, var(--neon-c)) 30%, transparent);
        flex-shrink: 0;
    }

    .game-nav__title {
        font-family: var(--header-font);
        font-size: var(--text-sm);
        letter-spacing: 0.1em;
        text-transform: uppercase;
        color: var(--game-text-color, var(--game-primary, var(--neon-c)));
        text-shadow:
            0 0 8px color-mix(in hsl, var(--game-primary, var(--neon-c)) 53%, transparent),
            0 0 24px color-mix(in hsl, var(--game-primary, var(--neon-c)) 20%, transparent);
    }

    .game-nav__close {
        background: transparent;
        border: 1px solid var(--border);
        color: var(--color);
        width: var(--icon-md);
        height: var(--icon-md);
        cursor: pointer;
        display: grid;
        place-items: center;
        font-size: var(--text-md);
        line-height: 1;
        transition:
            border-color var(--speed),
            color var(--speed);

        &:hover {
            border-color: var(--neon-m);
            color: var(--neon-m);
        }
    }

    /* ── Game List ── */
    .game-nav__list {
        flex: 1;
        overflow-y: auto;
        padding: var(--space-2) 0;

        scrollbar-width: thin;
        scrollbar-color: var(--game-primary, var(--neon-c)) transparent;
    }

    .game-nav__item {
        display: flex;
        flex-direction: row;
        align-items: center;
        gap: var(--space-3);
        padding: var(--space-3);
        text-decoration: none;
        color: var(--color);
        border-left: 3px solid transparent;
        transition:
            background var(--speed),
            border-color var(--speed);

        &:hover {
            background: color-mix(in hsl, var(--game-primary, var(--neon-c)) 6%, transparent);
            border-left-color: var(--game-primary, var(--neon-c));
        }

        &.active {
            background: color-mix(in hsl, var(--game-primary, var(--neon-c)) 8%, transparent);
            border-left-color: var(--game-primary, var(--neon-c));

            .game-nav__item-name {
                color: var(--game-text-color, var(--game-primary, var(--neon-c)));
                text-shadow:
                    0 0 8px color-mix(in hsl, var(--game-text-color, var(--game-primary, var(--neon-c))) 53%, transparent),
                    0 0 24px color-mix(in hsl, var(--game-text-color, var(--game-primary, var(--neon-c))) 20%, transparent);
            }
        }
    }

    .game-nav__item-logo {
        flex-shrink: 0;
        width: 64px;
        height: 36px;
        object-fit: contain;
        object-position: left center;
        opacity: 0.85;
        transition: opacity var(--speed);

        .game-nav__item:hover &,
        .game-nav__item.active & {
            opacity: 1;
        }
    }

    .game-nav__item-info {
        display: flex;
        flex-direction: column;
        gap: var(--space-1);
        min-width: 0;
    }

    .game-nav__item-name {
        font-family: var(--header-font);
        font-size: var(--text-sm);
        letter-spacing: 0.06em;
        text-transform: uppercase;
        transition: color var(--speed);
        white-space: nowrap;
        overflow: hidden;
        text-overflow: ellipsis;
    }

    .game-nav__item-tags {
        font-size: var(--text-xs);
        opacity: 0.45;
        letter-spacing: 0.04em;
        text-transform: uppercase;
        white-space: nowrap;
        overflow: hidden;
        text-overflow: ellipsis;
    }
`

export default styles
