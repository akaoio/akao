import { css } from "/core/UI.js"

export const styles = css`
    :host {
        label:has(input[type="checkbox"]) {
            display: inline-flex;
            align-items: center;
            gap: var(--space);
            cursor: pointer;
        }

        input[type="checkbox"] {
            --size: var(--unit-5);
            display: flex;
            align-items: center;
            justify-content: center;
            position: relative;
            appearance: none;
            width: var(--size);
            height: var(--size);
            margin: 0;
            border: none;

            &::before {
                content: "";
                position: absolute;
                width: 100%;
                height: 100%;
                border: var(--border);
            }

            &::after {
                content: "";
                position: absolute;
                width: 50%;
                height: 50%;
            }

            &:checked {
                border: none;
                &::before {
                    border: var(--border-accent);
                }
                &::after {
                    background: var(--background-accent);
                }
            }

            &:hover {
                border: none;
            }
        }
    }
`

export default styles