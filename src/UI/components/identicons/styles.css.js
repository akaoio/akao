import { css } from "/core/UI.js"

export const styles = css`
    :host {
        display: block;
        width: 100%;
        height: 100%;
        position: relative;
    }

    #container {
        width: 100%;
        height: 100%;
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
                    color: var(--neon-c);
                    opacity: 1;
                    transform: scale(1.2);
                    outline: 1px solid color-mix(in hsl, var(--neon-c) 70%, transparent);
                    outline-offset: var(--space-1);
                    box-shadow: var(--glow-c);
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
                    color: color-mix(in hsl, var(--neon-c) 60%, var(--color));
                }
            }
        }
    }

    @media (prefers-reduced-motion: reduce) {
        #container { scroll-behavior: auto; }
        #container .item label { transition: none; }
    }
`

export default styles
