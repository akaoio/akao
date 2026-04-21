/**
 * Breakpoint constants for responsive design.
 *
 * CSS custom properties cannot be used inside @media conditions, so breakpoints
 * live here as JS constants and are interpolated into css`` tagged templates.
 *
 * Three named directions:
 *
 *   mq.smUp   — sm and wider   (min-width)
 *   mq.mdDown — md and below   (max-width, exclusive)
 *   mq.mdOnly — md tier only   (between md and lg)
 *
 * Usage:
 *   import { mq } from "/UI/css/breakpoints.js"
 *
 *   const styles = css`
 *     .foo { display: block; }
 *     @media ${mq.smUp} { .foo { display: grid; } }
 *     @media ${mq.mdUp} { .foo { grid-template-columns: repeat(2, 1fr); } }
 *   `
 *
 * bp is exported for the rare case where a raw px value is needed
 * (e.g. hardcoded grid thresholds). Prefer mq in all style files.
 */

/** Raw pixel values — use mq in styles; bp only when a literal px value is required. */
export const bp = {
    xs:  480,   // small phones
    sm:  576,   // phones / small tablets
    md:  768,   // tablets
    lg:  992,   // desktop
    xl:  1200,  // wide desktop
    xxl: 1400,  // ultra-wide
}

/**
 * Named media query strings — interpolate into @media conditions.
 *
 * *Up   — this tier and wider   (mobile-first enhancements, prefer these)
 * *Down — this tier and below   (use sparingly for overrides)
 * *Only — exactly this tier     (use rarely for pinpoint tweaks)
 *
 * Down uses an exclusive upper edge (bp − 0.02px) to avoid overlap with *Up.
 */
export const mq = {
    // ── Up (min-width) ───────────────────────────────────────────────────────
    // xs has no Up variant — below sm is the default (no query needed)
    smUp:  `(min-width: ${bp.sm}px)`,       // ≥576px
    mdUp:  `(min-width: ${bp.md}px)`,       // ≥768px
    lgUp:  `(min-width: ${bp.lg}px)`,       // ≥992px
    xlUp:  `(min-width: ${bp.xl}px)`,       // ≥1200px
    xxlUp: `(min-width: ${bp.xxl}px)`,      // ≥1400px

    // ── Down (max-width, exclusive) ──────────────────────────────────────────
    xsDown:  `(max-width: ${bp.xs - 0.02}px)`,   // <480px
    smDown:  `(max-width: ${bp.sm - 0.02}px)`,   // <576px
    mdDown:  `(max-width: ${bp.md - 0.02}px)`,   // <768px
    lgDown:  `(max-width: ${bp.lg - 0.02}px)`,   // <992px
    xlDown:  `(max-width: ${bp.xl - 0.02}px)`,   // <1200px
    xxlDown: `(max-width: ${bp.xxl - 0.02}px)`,  // <1400px

    // ── Only (single tier) ───────────────────────────────────────────────────
    xsOnly:  `(max-width: ${bp.sm - 0.02}px)`,
    smOnly:  `(min-width: ${bp.sm}px) and (max-width: ${bp.md - 0.02}px)`,
    mdOnly:  `(min-width: ${bp.md}px) and (max-width: ${bp.lg - 0.02}px)`,
    lgOnly:  `(min-width: ${bp.lg}px) and (max-width: ${bp.xl - 0.02}px)`,
    xlOnly:  `(min-width: ${bp.xl}px) and (max-width: ${bp.xxl - 0.02}px)`,
    // xxlOnly has no upper bound — identical to xxlUp
}
