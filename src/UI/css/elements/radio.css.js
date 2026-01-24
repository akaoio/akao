import { css } from "/core/UI.js"

export const styles = css`
    :host {
        label:has(input[type="radio"]) {
            display: inline-flex;
            align-items: center;
            gap: var(--space);
            cursor: pointer;
        }

        input[type="radio"] {
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
            cursor: pointer;

            &::before {
                content: "";
                position: absolute;
                width: 100%;
                height: 100%;
                border: var(--border);
                box-sizing: inset;
                border-radius: 50%;
            }

            &::after {
                content: "";
                position: absolute;
                width: 50%;
                height: 50%;
                border-radius: 50%;
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
                &::before {
                    border: var(--border-accent);
                }
            }
        }
    }
`

export default styles