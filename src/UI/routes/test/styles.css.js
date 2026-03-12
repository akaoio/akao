import { css } from "/core/UI.js"

export const styles = css`
    :host {
        display: block;
        color: var(--color);

        #test-root {
            max-width: 860px;
            min-width: 500px;
            font-family: var(--font);
            display: flex;
            flex-direction: column;
            gap: var(--space);
            margin: auto;
        }

        #summary {
            display: flex;
            align-items: center;
            gap: var(--space);
            padding: var(--space) var(--space-lg);
            border: var(--border);
            background: color-mix(in hsl, var(--color) 5%, transparent);
            flex-wrap: wrap;

            h1 {
                margin: 0;
                font-size: var(--text-lg);
                font-weight: 700;
                flex: 1;
            }

            .count {
                font-size: var(--text-sm);
                padding: 2px 10px;
                font-weight: 700;

                &.passed { background: #22c55e28; color: #4ade80; }
                &.failed { background: #ef444428; color: #f87171; }
                &.skipped { background: #f59e0b28; color: #fbbf24; }
            }

            .progress-bar {
                width: 100%;
                height: 3px;
                background: color-mix(in hsl, var(--color) 15%, transparent);
                overflow: hidden;

                .fill {
                    height: 100%;
                    transition: width 0.3s ease;
                    background: #4ade80;

                    &.has-failures { background: #f87171; }
                }
            }

            .actions {
                display: flex;
                gap: var(--space-sm);

                button {
                    padding: 3px 12px;
                    border: var(--border);
                    background: transparent;
                    color: var(--color);
                    cursor: pointer;
                    font-size: var(--text-sm);
                    font-family: inherit;
                    opacity: 0.8;

                    &:hover { opacity: 1; background: color-mix(in hsl, var(--color) 8%, transparent); }
                }
            }
        }

        #suites {
            display: flex;
            flex-direction: column;
            gap: var(--space);
        }

        .suite {
            margin-bottom: calc(var(--space-sm) / 2);
            border: var(--border);
            overflow: hidden;

            .suite-header {
                display: flex;
                align-items: center;
                gap: var(--space);
                padding: calc(var(--space-sm) * 1.5) var(--space);
                cursor: pointer;
                user-select: none;
                background: color-mix(in hsl, var(--color) 6%, transparent);

                &:hover { background: color-mix(in hsl, var(--color) 10%, transparent); }

                .arrow {
                    transition: transform 0.2s;
                    font-size: 0.65rem;
                    opacity: 0.5;
                }
                &.open .arrow { transform: rotate(90deg); }

                .suite-name {
                    flex: 1;
                    font-weight: 600;
                    font-size: var(--text-sm);
                }

                .badge {
                    font-size: calc(var(--text-sm) * 0.9);
                    padding: 1px 8px;
                    font-weight: 700;

                    &.all-pass { background: #22c55e28; color: #4ade80; }
                    &.has-fail { background: #ef444428; color: #f87171; }
                    &.interactive { background: #a855f728; color: #c084fc; }
                    &.running { background: #3b82f628; color: #60a5fa; }
                }
            }

            .suite-tests {
                display: none;
                border-top: var(--border);

                &.visible { display: block; }
            }

            .test-row {
                display: flex;
                align-items: flex-start;
                flex-wrap: wrap;
                gap: var(--space-sm);
                padding: calc(var(--space-sm) * 1.5) var(--space);
                border-bottom: var(--border);
                font-size: var(--text-sm);
                opacity: 0.8;

                &:last-child { border-bottom: none; }
                &.fail { opacity: 1; }

                .icon {
                    flex-shrink: 0;
                    width: 14px;
                    text-align: center;
                    font-size: 0.75rem;
                    margin-top: 1px;
                }

                .name { flex: 1; min-width: 0; }

                .run-btn {
                    padding: 0 8px;
                    font-size: calc(var(--text-sm) * 0.85);
                    border: 1px solid #a855f7;
                    background: #a855f714;
                    color: #c084fc;
                    cursor: pointer;
                    font-family: inherit;

                    &:hover { background: #a855f730; }
                }

                .error-msg {
                    flex-basis: 100%;
                    margin-left: calc(14px + var(--space-sm));
                    padding: var(--space-sm) var(--space);
                    background: #ef444414;
                    border-left: 2px solid #f87171;
                    color: #f87171;
                    font-family: monospace;
                    font-size: calc(var(--text-sm) * 0.9);
                    word-break: break-all;
                    white-space: pre-wrap;
                }
            }

            .test-row.pass .icon { color: #4ade80; }
            .test-row.fail .icon { color: #f87171; }
            .test-row.pending .icon { color: #c084fc; }
            .test-row.skip .icon { color: color-mix(in hsl, var(--color) 40%, transparent); }
            .test-row.running .icon { color: #60a5fa; }
        }
    }
`

export default styles
