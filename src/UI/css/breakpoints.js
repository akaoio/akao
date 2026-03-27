/**
 * Breakpoint constants for responsive design.
 *
 * CSS custom properties cannot be used inside @media conditions, so breakpoints
 * live here as JS constants and are interpolated into css`` tagged templates.
 *
 * Usage:
 *   import { mq, bp } from "/UI/css/breakpoints.js"
 *
 *   const styles = css`
 *     .foo { display: grid; }
 *     @media ${mq.sm} { .foo { display: block; } }
 *   `
 *
 * Convention: all existing code uses max-width (desktop-first).
 * New code should prefer min-width (mobile-first) using the *Up variants.
 */

/** Raw pixel values — use these when you need arithmetic (e.g. min-width: ${bp.sm + 1}px) */
export const bp = {
    xs: 480,    // small phones
    sm: 767,    // phones / small tablets boundary
    md: 1023,   // tablets / desktop boundary
    lg: 1280,   // wide desktop

    // Component-specific grid thresholds (catalog grid in game/[game])
    grid1: 540, // 1-column catalog grid breakpoint
    grid2: 849, // 2-column catalog grid breakpoint
}

/** Pre-built media query strings — interpolate directly into @media rules */
export const mq = {
    // max-width (desktop-first) — matches screens AT OR BELOW this width
    xs:   `(max-width: ${bp.xs}px)`,
    sm:   `(max-width: ${bp.sm}px)`,
    md:   `(max-width: ${bp.md}px)`,
    lg:   `(max-width: ${bp.lg}px)`,

    // min-width (mobile-first) — matches screens ABOVE this width
    smUp: `(min-width: ${bp.xs + 1}px)`,
    mdUp: `(min-width: ${bp.sm + 1}px)`,
    lgUp: `(min-width: ${bp.md + 1}px)`,

    // Grid-specific
    grid2: `(max-width: ${bp.grid2}px)`,
    grid1: `(max-width: ${bp.grid1}px)`,
}
