import { css } from "/core/UI.js"
import { bp } from "/UI/css/breakpoints.js"

export const styles = css`
    :host {
        header {
            position: fixed;
            top: 0;
            z-index: 50;
            width: 100%;
            background: transparent;

            &::before {
                content: "";
                position: absolute;
                inset: 0;
                pointer-events: none;
                background: linear-gradient(
                    to bottom,
                    var(--background) 0%,
                    color-mix(in hsl, var(--background) 0%, transparent 100%) 100%
                );
                backdrop-filter: var(--header-backdrop, none);
                -webkit-backdrop-filter: var(--header-backdrop, none);
                mask-image: linear-gradient(to bottom, #000 0%, #000 60%, transparent 100%);
                -webkit-mask-image: linear-gradient(to bottom, #000 0%, #000 60%, transparent 100%);
            }

            nav {
                max-height: var(--header-height);
                display: flex;
                align-items: center;
                justify-content: space-between;
                padding: var(--space);
                position: relative;
                z-index: 1;

                & a,
                & div {
                    display: flex;
                    gap: var(--space);
                }
            }

            .brand {
                max-width: 8rem;
                color: var(--brand-color);
                display: flex;
                filter: drop-shadow(var(--glow-c, none));
            }

            .games {
                display: none;
                @media (max-width: ${bp.sm}px) {
                    display: flex;
                }
            }
        }
    }
`

export default styles
