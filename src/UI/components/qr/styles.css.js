import { css } from "/core/UI.js"

export const styles = css`
    :host {
        display: flex;
        position: relative;
        width: 100%;
        height: 100%;

        #scanner:not([hidden]) {
            position: relative;
            width: 100%;
            height: 100%;
        }

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

        #controls {
            display: flex;
            gap: var(--space);
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