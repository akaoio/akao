/**
 * Breakpoint constants for responsive design.
 *
 * CSS custom properties cannot be used inside @media conditions, so breakpoints
 * live here as JS constants and are interpolated into css`` tagged templates.
 *
 * Usage:
 *   import { bp } from "/UI/css/breakpoints.js"
 *
 *   const styles = css`
 *     .foo { display: grid; }
 *     @media (max-width: ${bp.sm}px) { .foo { display: block; } }
 *   `
 */

/** Raw pixel values — interpolate directly into @media conditions */
export const bp = {
    xs: 480,    // small phones
    sm: 767,    // phones / small tablets boundary
    md: 1023,   // tablets / desktop boundary
    lg: 1280,   // wide desktop

    // Component-specific grid thresholds (catalog grid in game/[game])
    grid1: 540, // 1-column catalog grid breakpoint
    grid2: 849, // 2-column catalog grid breakpoint
}
