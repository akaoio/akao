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
            left: var(--space);
            top: var(--space);
            opacity: 0.5;
        }

        .controls {
            display: flex;
            position: absolute;
            gap: var(--space);
            align-items: center;
            left: 50%;
            transform: translateX(-50%);
            bottom: var(--space);
        }
    }
`

export default styles