import { css } from "/core/UI.js"

export const styles = css`
    :host {
        button {
            display: inline-flex;
            align-items: center;
            justify-content: center;
            gap: var(--space);
            background: var(--btn-bg, var(--background));
            border: var(--btn-border, var(--border));
            border-radius: var(--btn-radius, var(--radius));
            padding: var(--btn-padding, var(--space));
            color: var(--btn-color, var(--color));
            cursor: pointer;
            white-space: nowrap;
            box-sizing: border-box;
            font-family: var(--btn-font, var(--font));
            font-size: var(--btn-font-size, var(--text-sm));
            font-weight: var(--btn-font-weight, normal);
            letter-spacing: var(--btn-letter-spacing, normal);
            text-transform: var(--btn-text-transform, none);
            transition: background var(--speed), color var(--speed), border-color var(--speed), box-shadow var(--speed);
            &.full {
                width: 100%;
            }
            &:has(.icon[data-src]) {
                justify-content: stretch;
            }
            .icon:not([data-src]) {
                display: none;
            }
            .icon {
                width: var(--icon-sm);
                aspect-ratio: 1;
            }
            &:disabled {
                opacity: 0.35;
                cursor: not-allowed;
                pointer-events: none;
            }
        }

        button:not(:disabled):hover {
            background: var(--btn-bg-hover, var(--background-inverted));
            color: var(--btn-color-hover, var(--color-inverted));
            border-color: var(--btn-border-color-hover, inherit);
            box-shadow: var(--btn-glow-hover, none);
        }
    }
`

export default styles
