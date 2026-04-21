import { css } from "/core/UI.js"
import { bp, mq } from "/UI/css/breakpoints.js"

/**
 * Shared grid patterns. Import and apply the class to any container.
 *
 * .grid          Fixed column count, collapses 3→2→1 at grid2/grid1 breakpoints.
 *                --grid-cols   (default 3)   number of columns
 *                --grid-gap    (default --space-3)  uniform gap
 *                --grid-gap-x  column-gap override
 *                --grid-gap-y  row-gap override
 *
 * .grid--fluid   Auto-fill fluid columns, no breakpoint overrides needed.
 *                --grid-min    (default 16rem)  minimum column width
 *                --grid-gap    shared gap token
 *
 * .grid--sidebar Fixed sidebar + 1fr main, collapses to 1-col at sm.
 *                --sidebar-width  (default 15rem)  sidebar column width
 *                --grid-gap       shared gap token
 *
 * .grid--table   Column-contract table rows via --table-cols.
 *                --table-cols  grid-template-columns value (required)
 *                Each table defines its own responsive collapse strategy.
 */
export const grid = css`
    .grid {
        display: grid;
        grid-template-columns: repeat(var(--grid-cols, 3), 1fr);
        column-gap: var(--grid-gap-x, var(--grid-gap, var(--space-3)));
        row-gap: var(--grid-gap-y, var(--grid-gap, var(--space-3)));

        @media (max-width: 849px) {
            grid-template-columns: repeat(min(var(--grid-cols, 3), 2), 1fr);
        }

        @media (max-width: 540px) {
            grid-template-columns: 1fr;
        }
    }

    .grid--fluid {
        display: grid;
        grid-template-columns: repeat(auto-fill, minmax(var(--grid-min, 16rem), 1fr));
        column-gap: var(--grid-gap-x, var(--grid-gap, var(--space-3)));
        row-gap: var(--grid-gap-y, var(--grid-gap, var(--space-3)));
    }

    .grid--sidebar {
        display: grid;
        grid-template-columns: var(--sidebar-width, 15rem) 1fr;
        column-gap: var(--grid-gap-x, var(--grid-gap, var(--space-3)));
        row-gap: var(--grid-gap-y, var(--grid-gap, var(--space-3)));

        @media ${mq.sm} {
            grid-template-columns: 1fr;
        }
    }

    .grid--table {
        display: grid;
        grid-template-columns: var(--table-cols);
        align-items: center;
    }
`

export default grid
