import { css } from "/core/UI.js"

export const styles = css`
    :host {
        display: flex;
        flex-direction: column;
        width: 100%;
        height: 100%;
    }

    #container {
        flex: 1;
        min-height: 0;
        width: 100%;
        display: flex;
        flex-direction: row;
        align-items: center;
        gap: var(--space-3);
        overflow-x: auto;
        scroll-snap-type: x mandatory;
        scroll-behavior: smooth;
        /* vertical padding absorbs the scale(1.2) of selected item */
        padding: var(--identicons-pad-v, var(--space-4)) var(--space-5);
        box-sizing: border-box;

        /* hide scrollbar cross-browser */
        scrollbar-width: none;
        &::-webkit-scrollbar { display: none; }

        /* edge fade — items dissolve into background at both sides */
        mask-image: linear-gradient(
            to right,
            transparent 0,
            black var(--space-5),
            black calc(100% - var(--space-5)),
            transparent 100%
        );
        -webkit-mask-image: linear-gradient(
            to right,
            transparent 0,
            black var(--space-5),
            black calc(100% - var(--space-5)),
            transparent 100%
        );

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
                    box-shadow: 0 0 8px hsl(var(--item-hue) 100% 65% / 53%), 0 0 24px hsl(var(--item-hue) 100% 65% / 20%);
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

    @media (prefers-reduced-motion: reduce) {
        #container { scroll-behavior: auto; }
        #container .item label { transition: none; }
    }

    #status {
        display: flex;
        align-items: center;
        gap: var(--space-2);
        padding: var(--space-1) var(--space-5);
        font-size: var(--text-xs);
        color: var(--color);
        opacity: 0.7;
        user-select: none;
        pointer-events: none;
        flex-shrink: 0;
    }

    .status-item {
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

    .status-num--saved  { color: var(--color-accent); }
    .status-num--preview { color: var(--neon-g); }

    .status-arrow {
        opacity: 0.4;
        font-size: 0.85em;
    }

    .status-sep {
        opacity: 0.25;
        margin: 0 var(--space-1);
    }

    .status-total {
        opacity: 0.45;
        font-variant-numeric: tabular-nums;
    }
`

export default styles
