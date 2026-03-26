import { css } from "/core/UI.js"

const styles = css`
    :host {
        --icon: var(--icon-lg);
        display: flex;
        position: absolute;
        top: 0;
        height: 100%;
        width: 100%;
        video {
            width: 100%;
            height: 100%;
            object-fit: cover;
        }

        #status {
            position: absolute;
            left: 50%;
            top: var(--space);
            transform: translateX(-50%);
            width: max-content;
            max-width: calc(100% - (var(--space) * 2));
            z-index: 2;
            text-align: center;
            white-space: normal;
            overflow-wrap: anywhere;
            opacity: 0.5;
        }

        #controls {
            display: flex;
            position: absolute;
            gap: var(--space);
            align-items: center;
            left: 50%;
            transform: translateX(-50%);
            bottom: var(--space);
            .controls {
                display: flex;
                gap: var(--space);
            }
        }
    }
`

export default styles