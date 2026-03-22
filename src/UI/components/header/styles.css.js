import { css } from "/core/UI.js"

export const styles = css`
    @keyframes header-nav-sweep {
        0% {
            background-position: 0% 0;
        }
        100% {
            background-position: 100% 0;
        }
    }

    :host {
        header {
            position: fixed;
            top: 0;
            z-index: 50;
            width: 100%;
            background: var(--header-background, var(--background));
            backdrop-filter: var(--header-backdrop, none);
            -webkit-backdrop-filter: var(--header-backdrop, none);

            nav {
                max-height: var(--header-height);
                display: flex;
                align-items: center;
                justify-content: space-between;
                padding: var(--space);
                position: relative;

                &::after {
                    content: "";
                    position: absolute;
                    bottom: 0;
                    left: 0;
                    width: 100%;
                    height: 1px;
                    background: linear-gradient(90deg, transparent 0%, #00e5ff 25%, #9c3fc7 55%, #ff2d78 80%, transparent 100%);
                    background-size: 200% 100%;
                    animation: header-nav-sweep 4s ease-in-out infinite alternate;
                }

                & a,
                & div {
                    display: flex;
                    gap: var(--space);
                }
            }

            .brand {
                max-width: 8rem;
                color: var(--neon-c);
                display: flex;
                filter: drop-shadow(var(--glow-c, none));
            }

            .game-nav-btn {
                display: none;
                background: transparent;
                border: none;
                color: var(--color);
                cursor: pointer;
                width: var(--icon-md);
                height: var(--icon-md);
                align-items: center;
                justify-content: center;
                padding: 0;
                transition: color var(--speed);

                &:hover {
                    color: var(--neon-c);
                }

                @media (max-width: 767px) {
                    display: flex;
                }
            }
        }
    }
`

export default styles
