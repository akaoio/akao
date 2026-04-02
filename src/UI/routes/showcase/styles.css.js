import { css } from "/core/UI.js"
import typographyStyles from "./stories/typography.css.js"

export const styles = css`
    :host {
        .showcase {
            display: grid;
            grid-template-columns: 14rem 1fr;
            height: calc(100vh - var(--header-height) - var(--footer-height) - var(--space-6));
            overflow: hidden;
            border: var(--border);
        }

        #sidebar {
            overflow-y: auto;
            border-right: var(--border);
            padding: var(--space-2) 0;
            display: flex;
            flex-direction: column;

            .nav-group {
                padding: var(--space) var(--space-3) var(--space-1);
                font-size: var(--text-xs);
                letter-spacing: 0.15em;
                text-transform: uppercase;
                color: var(--neon-c);
                text-shadow: 0 0 8px color-mix(in srgb, var(--neon-c) 60%, transparent);
                opacity: 0.85;
                font-family: var(--header-font);
            }

            .nav-item {
                display: block;
                padding: var(--space) var(--space-3);
                font-family: var(--header-font);
                font-size: var(--text-xs);
                letter-spacing: 0.08em;
                text-transform: uppercase;
                text-decoration: none;
                color: var(--color);
                opacity: 0.5;
                border-left: calc(var(--border-width) * 2) solid transparent;
                transition:
                    opacity var(--speed) ease-in-out,
                    color var(--speed) ease-in-out,
                    border-color var(--speed) ease-in-out;

                &:hover {
                    opacity: 0.8;
                }

                &.active {
                    opacity: 1;
                    border-left-color: var(--color-accent);
                    color: var(--color-accent);
                    text-shadow: var(--section-label-shadow, none);
                }
            }
        }

        #canvas {
            overflow-y: auto;
            padding: var(--space-4);

            .canvas-title {
                font-family: var(--header-font);
                font-size: var(--text-lg);
                font-weight: 700;
                letter-spacing: 0.08em;
                text-transform: uppercase;
                margin-bottom: var(--space-4);
                padding-bottom: var(--space-2);
                border-bottom: var(--border);
                color: var(--section-label-color, var(--color));
                text-shadow: var(--section-label-shadow, none);
            }

            .story-locale-bar {
                display: flex;
                align-items: center;
                gap: var(--space-2);
                padding: var(--space-2) 0;
                margin-bottom: var(--space-3);
                border-bottom: var(--border);
            }

            .story-locale-label {
                font-family: var(--header-font);
                font-size: var(--text-xs);
                text-transform: uppercase;
                letter-spacing: 0.1em;
                opacity: 0.4;
            }

            .story-locale-select {
                --select-width: 14rem;
            }

            .stories-grid {
                display: grid;
                grid-template-columns: repeat(auto-fill, minmax(16rem, 1fr));
                gap: var(--space-3);
            }

            .story-card {
                border: var(--border);
                border-left: var(--item-border-left, var(--border));
                display: flex;
                flex-direction: column;
                overflow: hidden;

                &.wide {
                    grid-column: 1 / -1;
                }
            }

            .story-name {
                font-family: var(--header-font);
                font-size: var(--text-xs);
                letter-spacing: 0.1em;
                text-transform: uppercase;
                padding: var(--space) var(--space-2);
                border-bottom: var(--border);
                color: var(--color-accent);
                opacity: 0.8;
            }

            .story-preview {
                padding: var(--space-3);
                display: flex;
                align-items: center;
                justify-content: center;
                flex-wrap: wrap;
                gap: var(--space);
                min-height: var(--space-9);
                background: var(--item-background, transparent);
            }

            .story-code-toggle {
                display: flex;
                align-items: center;
                justify-content: space-between;
                width: 100%;
                padding: var(--space) var(--space-2);
                border: none;
                border-top: var(--border);
                background: var(--background);
                color: var(--color-accent);
                opacity: 0.8;
                cursor: pointer;
                font-family: var(--header-font);
                font-size: var(--text-xs);
                letter-spacing: 0.08em;
                text-transform: uppercase;
                transition: opacity var(--speed) ease-in-out;

                &:hover {
                    opacity: 0.7;
                }

                .story-code-toggle-arrow {
                    opacity: 0.6;
                }
            }

            .story-code {
                font-family: "Share Tech Mono", "Courier New", monospace;
                font-size: var(--text-xs);
                line-height: 1.6;
                padding: var(--space-2);
                background: var(--background);
                border-top: var(--border);
                white-space: pre-wrap;
                word-break: break-all;
                color: var(--section-label-color, var(--color-accent));
                opacity: 0.7;
                display: none;

                .story-card.code-open & {
                    display: block;
                }
            }

            .story-preview {
                .svg-sm {
                    width: var(--icon-md);
                }
                .svg-md {
                    width: var(--icon-lg);
                }
                .svg-lg {
                    width: var(--icon-xlg);
                }
                .svg-color {
                    color: var(--color);
                }
                .svg-color-accent {
                    color: var(--color-accent);
                }

                .modal-body {
                    padding: var(--space-2);
                }

                .locale-grid {
                    list-style: none;
                    margin: 0;
                    padding: 0;
                    display: grid;
                    grid-template-columns: repeat(3, 1fr);
                    gap: var(--space) var(--space-4);
                    font-size: var(--text-sm);
                }

                .glitch-text {
                    font-family: var(--header-font);
                    font-size: 3rem;
                    font-weight: 700;
                    text-transform: lowercase;
                    position: relative;
                    margin: 0;
                    color: #fff;
                }

                .glitch-text span {
                    position: absolute;
                    top: 0;
                    left: 0;
                    color: #fff;
                }

                .glitch-text span:first-child {
                    clip-path: polygon(0 0, 100% 0, 100% 45%, 0 45%);
                    transform: translate(-0.025em, -0.0125em);
                    opacity: 0.8;
                }

                .glitch-text span:last-child {
                    clip-path: polygon(0 80%, 100% 20%, 100% 100%, 0 100%);
                    transform: translate(0.0125em, 0.025em);
                    opacity: 0.8;
                }

                .glitch-text.is-glitching {
                    text-shadow:
                        0.05em 0 0 rgba(255, 0, 0, 0.75),
                        -0.025em -0.05em 0 rgba(0, 255, 0, 0.75),
                        0.025em 0.05em 0 rgba(0, 0, 255, 0.75);
                    animation: glitch-text 1200ms infinite;
                }

                .glitch-text.is-glitching span:first-child {
                    animation: glitch-text 1600ms infinite;
                }

                .glitch-text.is-glitching span:last-child {
                    animation: glitch-text 900ms infinite;
                }
            }
        }
    }

    @keyframes glitch-text {
        0%,
        14% {
            text-shadow:
                0.05em 0 0 rgba(255, 0, 0, 0.75),
                -0.05em -0.025em 0 rgba(0, 255, 0, 0.75),
                -0.025em 0.05em 0 rgba(0, 0, 255, 0.75);
        }
        15%,
        49% {
            text-shadow:
                -0.05em -0.025em 0 rgba(255, 0, 0, 0.75),
                0.025em 0.025em 0 rgba(0, 255, 0, 0.75),
                -0.05em -0.05em 0 rgba(0, 0, 255, 0.75);
        }
        50%,
        99% {
            text-shadow:
                0.025em 0.05em 0 rgba(255, 0, 0, 0.75),
                0.05em 0 0 rgba(0, 255, 0, 0.75),
                0 -0.05em 0 rgba(0, 0, 255, 0.75);
        }
        100% {
            text-shadow:
                -0.025em 0 0 rgba(255, 0, 0, 0.75),
                -0.025em -0.025em 0 rgba(0, 255, 0, 0.75),
                -0.025em -0.05em 0 rgba(0, 0, 255, 0.75);
        }
    }

    @keyframes glitch-tiktok {
        0%,
        14% {
            filter: drop-shadow(4px 0 0 var(--glitch-ch1)) drop-shadow(-4px -2px 0 var(--glitch-ch2));
        }
        15%,
        49% {
            filter: drop-shadow(-4px -2px 0 var(--glitch-ch1)) drop-shadow(2px 2px 0 var(--glitch-ch2));
        }
        50%,
        99% {
            filter: drop-shadow(2px 4px 0 var(--glitch-ch1)) drop-shadow(4px 0 0 var(--glitch-ch2));
        }
        100% {
            filter: drop-shadow(-2px 0 0 var(--glitch-ch1)) drop-shadow(-2px -2px 0 var(--glitch-ch2));
        }
    }

    @media (prefers-reduced-motion: reduce) {
        * {
            animation-duration: 1ms !important;
            animation-iteration-count: 1 !important;
        }
    }

    ${typographyStyles}
`

export default styles
