import { css } from "/core/UI.js"

export const styles = css`
    :host {
        #qr {
            display: flex;
            background: var(--color-accent);
            padding: var(--space);
            &:empty {
                display: none;
            }
        }

        #scanner[hidden] {
            display: none;
        }

        .scanner-shell {
            display: grid;
            gap: var(--space-2);
            width: 100%;
            max-width: 34rem;
        }

        .scanner-panel {
            padding: var(--space-2);
            border-radius: var(--radius);
            background: var(--background-focus);
            border: var(--border);
            display: grid;
            gap: var(--space);
        }

        .actions {
            display: flex;
            gap: var(--space);
            flex-wrap: wrap;
        }

        .actions ui-button {
            display: block;
        }

        #status {
            color: var(--color);
            min-height: calc(var(--text-sm) * 1.5);
            font-size: var(--text-sm);
            opacity: 0.8;
        }

        #result {
            margin: 0;
            padding: var(--space);
            min-height: 4rem;
            border-radius: var(--radius);
            background: var(--background);
            border: var(--border);
            color: var(--color);
            white-space: pre-wrap;
            word-break: break-word;
            font-size: var(--text-xs);
            line-height: 1.5;
        }
    }
`

export default styles