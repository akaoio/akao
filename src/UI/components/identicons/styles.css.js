import { css } from "/core/UI.js"
import { bp } from "/UI/css/breakpoints.js"

export const styles = css`
    :host {
        display: flex;
        flex-direction: column;
        width: 100%;
        height: 100%;
    }

    #scroll-wrap {
        flex: 1;
        min-height: 0;
        position: relative;
        overflow: hidden;
    }

    #container {
        width: 100%;
        height: 100%;
        display: flex;
        flex-direction: row;
        align-items: center;
        gap: var(--space-5);
        overflow-x: auto;
        scroll-snap-type: x mandatory;
        scroll-behavior: smooth;
        /* vertical padding absorbs the scale(1.2) of selected item; extra right room for scaled last item */
        padding: var(--identicons-pad-v, var(--space-6)) var(--space-9) var(--identicons-pad-v, var(--space-6)) var(--space-6);
        box-sizing: border-box;

        /* hide scrollbar on touch devices */
        scrollbar-width: none;
        &::-webkit-scrollbar { display: none; }

        @media (min-width: ${bp.sm + 1}px) {
            scrollbar-width: thin;
            scrollbar-color: color-mix(in hsl, var(--color) 25%, transparent) transparent;
            padding-bottom: var(--space-3);

            &::-webkit-scrollbar { display: block; height: 3px; }
            &::-webkit-scrollbar-track { background: transparent; }
            &::-webkit-scrollbar-thumb {
                background: color-mix(in hsl, var(--color) 25%, transparent);
                border-radius: 99px;
                &:hover { background: color-mix(in hsl, var(--color) 50%, transparent); }
            }
        }

        /* edge fade — items dissolve into background at both sides */
        mask-image: linear-gradient(to right, transparent 0, black var(--space-6), black calc(100% - var(--space-9)), transparent 100%);
        -webkit-mask-image: linear-gradient(to right, transparent 0, black var(--space-6), black calc(100% - var(--space-9)), transparent 100%);

        .item {
            flex-shrink: 0;
            scroll-snap-align: center;
            width: var(--icon-lg);
            height: var(--icon-lg);

            input[type="radio"] {
                display: none;

                &:checked + label {
                    color: hsl(var(--item-hue) 100% 65%);
                    opacity: 1;
                    transform: scale(1.2);
                    outline: 1px solid hsl(var(--item-hue) 100% 65% / 70%);
                    outline-offset: var(--space-1);
                    box-shadow:
                        0 0 8px hsl(var(--item-hue) 100% 65% / 53%),
                        0 0 24px hsl(var(--item-hue) 100% 65% / 20%);
                }
            }

            label {
                display: flex;
                align-items: center;
                justify-content: center;
                width: 100%;
                height: 100%;
                cursor: pointer;
                color: var(--color);
                opacity: 0.4;
                transition:
                    opacity var(--speed),
                    transform var(--speed),
                    color var(--speed),
                    outline-color var(--speed),
                    box-shadow var(--speed);

                &:hover {
                    opacity: 0.75;
                    color: hsl(var(--item-hue) 100% 65% / 60%);
                }
            }
        }
    }

    #loader {
        position: absolute;
        inset: 0;
        display: flex;
        align-items: center;
        justify-content: center;
        background: color-mix(in hsl, var(--background) 70%, transparent);
        backdrop-filter: blur(2px);
        -webkit-backdrop-filter: blur(2px);

        &[hidden] {
            display: none;
        }
    }

    .loader-spinner {
        width: 1.25rem;
        height: 1.25rem;
        border-radius: 50%;
        border: 2px solid transparent;
        border-top-color: var(--neon-g);
        border-right-color: color-mix(in hsl, var(--neon-g) 35%, transparent);
        animation: identicons-spin 0.7s linear infinite;
    }

    @keyframes identicons-spin {
        to {
            transform: rotate(360deg);
        }
    }

    @media (prefers-reduced-motion: reduce) {
        #container {
            scroll-behavior: auto;
        }
        #container .item label {
            transition: none;
        }
        .loader-spinner {
            animation: none;
            opacity: 0.5;
        }
    }

    #status {
        display: flex;
        align-items: center;
        justify-content: space-between;
        gap: var(--space-2);
        padding: 0 var(--space-3);
        font-size: var(--text-sm);
        color: var(--color);
        opacity: 0.7;
        user-select: none;
        pointer-events: none;
        flex-shrink: 0;
        min-height: 2.75rem;
        border-bottom: var(--border-width) solid color-mix(in hsl, var(--color) 10%, transparent);
    }

    .status-left {
        display: flex;
        align-items: center;
        gap: var(--space-2);
        flex: 1;
        min-width: 0;
        overflow: hidden;
        white-space: nowrap;
    }

    .status-right {
        display: flex;
        align-items: center;
        flex-shrink: 0;
        white-space: nowrap;
    }

    .status-item {
        &[hidden] { display: none; }
        display: flex;
        align-items: center;
        gap: var(--space-1);
    }

    .status-label {
        opacity: 0.5;
        font-size: 0.85em;
        text-transform: uppercase;
        letter-spacing: 0.06em;
    }

    .status-num {
        font-variant-numeric: tabular-nums;
        font-weight: 600;
    }

    .status-num--saved {
        color: var(--color-accent);
    }
    .status-num--preview {
        color: var(--neon-g);
    }

    .status-arrow {
        opacity: 0.4;
        font-size: 0.85em;
    }

    .status-sep {
        opacity: 0.25;
        margin: 0 var(--space-1);
    }

    .status-total-group {
        display: flex;
        align-items: center;
        gap: var(--space-1);
    }

    .status-total {
        opacity: 0.45;
        font-variant-numeric: tabular-nums;
    }

    .status-nav-btn {
        display: inline-flex;
        align-items: center;
        justify-content: center;
        background: none;
        border: none;
        min-width: 2.75rem;
        min-height: 2.75rem;
        padding: 0 var(--space-2);
        font-size: var(--text-lg);
        line-height: 1;
        color: var(--neon-g);
        opacity: 1;
        text-shadow: var(--glow-g);
        cursor: pointer;
        pointer-events: all;
        transition: opacity var(--speed);

        &:hover:not(:disabled) {
            opacity: 0.7;
        }

        &:active:not(:disabled) {
            opacity: 0.5;
        }

        &:disabled {
            opacity: 0.15;
            text-shadow: none;
            cursor: not-allowed;
        }
    }

    .actions__sep {
        display: block;
        width: 1px;
        height: 1.2em;
        background: color-mix(in hsl, var(--color) 20%, transparent);
        margin: 0 var(--space-1);
    }
`

export default styles
