import { css } from "/core/UI.js"

export const styles = css`
    :host {
        display: flex;

        #qr {
            display: flex;
            background: var(--color-accent);
            padding: var(--space);
            svg {
                width: 100%;
            }
            &:empty {
                display: none;
            }
        }

        #scanner[hidden] {
            display: none;
        }

        #result {
            display: flex;
            position: absolute;
            bottom: calc(var(--footer-height) + var(--space));
            left: 50%;
            transform: translateX(-50%);
            width: max-content;
            color: var(--color-accent);
            white-space: pre-wrap;
            word-break: break-word;

            &:empty {
                display: none;
            }
        }
    }
`

export default styles