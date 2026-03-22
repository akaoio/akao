import { css } from "/core/UI.js"

const styles = css`
    :host {
        display: block;
        color: var(--color);
    }

    .camera-shell {
        display: grid;
        gap: var(--space-2);
        width: 100%;
        max-width: 34rem;
    }

    .viewport {
        position: relative;
        overflow: hidden;
        border-radius: var(--radius);
        background: var(--background-focus);
        border: var(--border);
        aspect-ratio: 4 / 3;
    }

    video {
        width: 100%;
        height: 100%;
        object-fit: cover;
        display: block;
    }

    #badge {
        position: absolute;
        left: var(--space-2);
        bottom: var(--space-2);
        background: var(--background-focus);
        border: var(--border);
        color: var(--color);
        padding: var(--space-sm) var(--space);
        border-radius: var(--radius);
        font-size: var(--text-xs);
        opacity: 0.85;
    }

    .controls {
        display: flex;
        gap: var(--space);
        flex-wrap: wrap;
    }

    ui-button {
        display: block;
    }
`

export default styles