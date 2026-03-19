import { css } from "/core/UI.js"

export const styles = css`
    :host {
        /* ── Hero ── */
        .hero {
            display: flex;
            flex-direction: column;
            align-items: center;
            text-align: center;
            gap: var(--space-3);
            padding: var(--space-8) 0 var(--space-6);

            .hero__eyebrow {
                font-size: var(--text-xs);
                letter-spacing: 0.2em;
                text-transform: uppercase;
                color: var(--color-accent);
                text-shadow: var(--hero-eyebrow-shadow, none);
            }

            h1 {
                font-family: var(--header-font);
                font-size: clamp(3rem, 8vw, 6rem);
                font-weight: 900;
                letter-spacing: 0.08em;
                line-height: 1;
                color: var(--color);
                text-shadow: var(--hero-title-shadow, none);
            }

            .hero__sub {
                font-size: var(--text-lg);
                color: var(--color);
                opacity: 0.5;
                max-width: 36rem;
            }

            .hero__actions {
                display: flex;
                gap: var(--space-2);
                margin-top: var(--space-2);
            }
        }

        /* ── Section header ── */
        .section-header {
            display: flex;
            align-items: baseline;
            justify-content: space-between;
            position: relative;
            padding-bottom: var(--space-2);

            &::after {
                content: '';
                position: absolute;
                bottom: 0;
                left: 0;
                right: 0;
                height: var(--border-width, 1px);
                background: var(--section-line, var(--color));
                opacity: var(--section-line, 1) 0.3;
            }

            h2 {
                font-family: var(--header-font);
                font-size: var(--text-lg);
                font-weight: 700;
                letter-spacing: 0.06em;
                text-transform: uppercase;
                color: var(--section-label-color, var(--color));
                text-shadow: var(--section-label-shadow, none);
            }

            a {
                font-size: var(--text-xs);
                color: var(--color-accent);
                text-decoration: none;
                letter-spacing: 0.1em;
                text-transform: uppercase;
                cursor: pointer;
                &:hover { opacity: 0.7; }
            }
        }

        section {
            display: flex;
            flex-direction: column;
            gap: var(--space-3);
        }
    }
`

export default styles
