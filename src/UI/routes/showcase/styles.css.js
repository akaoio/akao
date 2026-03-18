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
                color: var(--color);
                opacity: 0.3;
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
                border-left: 2px solid transparent;
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
                font-size: var(--text-sm);
                padding: var(--space) var(--space-2);
                border: var(--border);
                background: transparent;
                color: inherit;
                cursor: pointer;
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
                min-height: 6rem;
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
                color: var(--color);
                opacity: 0.4;
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
                    width: 2rem;
                }
                .svg-md {
                    width: 3rem;
                }
                .svg-lg {
                    width: 8rem;
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
            }
        }
    }

    ${typographyStyles}
`

export default styles
