import { css } from "/core/UI.js"

export const styles = css`
    :host {
        input {
            padding: var(--space);
            box-sizing: border-box;
            border: var(--border);
            border-radius: var(--radius);
            background: var(--background);
            color: var(--color);
            width: 100%;
            font-size: var(--text);
            &::placeholder {
                color: var(--color);
                opacity: 0.25;
                font-size: smaller;
                font-weight: lighter;
                letter-spacing: var(--unit-1);
            }
            &:hover {
                &::placeholder {
                    opacity: 0.75;
                }
                border: var(--border-accent);
            }
            &:focus {
                &::placeholder {
                    opacity: 0.5;
                }
                border: var(--border-accent);
            }
        }
    }
`

export default styles
