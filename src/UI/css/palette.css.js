import { css } from "/core/UI.js"

/**
 * Primitive color palette — design system foundation layer.
 *
 * These are raw color stops, NOT semantic tokens.
 * Components and themes reference semantic tokens from vars.css.js / dark.css.js / light.css.js,
 * which in turn are grounded by these stops.
 *
 * Scale: 100 (lightest) → 900 (darkest)
 *
 * Families:
 *   --navy-*    Teal-blue-gray ramp. The structural backbone of the dark theme.
 *               Anchors: 900=#121c26, 800=#1a2733, 700=#233c4d, 200=#c8dde8
 *               Hue ~205°, saturation ~36%, lightness 11%→85%
 *
 *   --green-*   Dark teal-green ramp. Used for card headers, focus tints, active states.
 *               Derived from the navy hue shifted toward green (hue ~155°).
 *               Lightness mirrors the navy scale so they pair predictably.
 *
 *   --neutral-* Pure achromatic gray. Light mode surfaces, text on light bg, disabled states.
 *               Hue 0°, saturation 0%.
 *
 *   --slate-*   Cool de-saturated blue-gray. The outer gutter / app chrome behind cards.
 *               Slightly cooler and flatter than navy (hue ~215°, sat ~20%).
 *               Use for page-level backgrounds that need to recede behind panel surfaces.
 *
 *   --teal-*    Pure teal midpoint between navy and green (hue ~180°, sat ~36%).
 *               Used for token field borders, SELECT TOKEN box backgrounds, focus rings.
 */

export const styles = css`
    :root {
        /* ── Navy ─────────────────────────────────────────────────────────────
           H: 207  S: 36%
           100 → near-white tint   900 → deepest background
        ──────────────────────────────────────────────────────────────────── */
        --navy-100: hsl(207, 36%, 90%);
        --navy-200: hsl(207, 36%, 80%);   /* ≈ #c8dde8 — primary text (dark theme) */
        --navy-300: hsl(207, 36%, 65%);
        --navy-400: hsl(207, 36%, 50%);
        --navy-500: hsl(207, 36%, 38%);
        --navy-600: hsl(207, 36%, 30%);
        --navy-700: hsl(207, 36%, 22%);   /* ≈ #233c4d — borders */
        --navy-800: hsl(207, 36%, 15%);   /* ≈ #1a2733 — card / panel surface */
        --navy-900: hsl(207, 36%, 11%);   /* ≈ #121c26 — page background */

        /* ── Green ────────────────────────────────────────────────────────────
           H: 155  S: 36%
           Used for card header tints, active surface highlights, focus layers.
           Same lightness ladder as navy so the two families blend naturally.
        ──────────────────────────────────────────────────────────────────── */
        --green-100: hsl(155, 36%, 90%);
        --green-200: hsl(155, 38%, 80%);
        --green-300: hsl(155, 37%, 65%);
        --green-400: hsl(155, 37%, 50%);
        --green-500: hsl(155, 37%, 38%);
        --green-600: hsl(155, 37%, 30%);
        --green-700: hsl(155, 37%, 22%);  /* card header bg tint base */
        --green-800: hsl(155, 32%, 15%);
        --green-900: hsl(155, 36%, 10%);

        /* ── Neutral ──────────────────────────────────────────────────────────
           H: 0  S: 0%  (pure gray)
           Light mode surfaces, disabled states, text on white backgrounds.
        ──────────────────────────────────────────────────────────────────── */
        --neutral-100: hsl(0, 0%, 98%);   /* ≈ #f5f5f7 — light mode page bg */
        --neutral-200: hsl(0, 0%, 93%);
        --neutral-300: hsl(0, 0%, 85%);
        --neutral-400: hsl(0, 0%, 78%);   /* ≈ #c8c8c8 — symbol fill */
        --neutral-500: hsl(0, 0%, 60%);
        --neutral-600: hsl(0, 0%, 45%);
        --neutral-700: hsl(0, 0%, 27%);   /* ≈ #444446 — light mode body text */
        --neutral-800: hsl(0, 0%, 18%);
        --neutral-900: hsl(0, 0%, 12%);   /* ≈ #1d1d1f — light mode deep surface */

        /* ── Slate ────────────────────────────────────────────────────────────
           H: 215  S: 20%
           De-saturated cool blue-gray. Recedes behind navy panels.
           100 → pale cool white   900 → app chrome / outer gutter
        ──────────────────────────────────────────────────────────────────── */
        --slate-100: hsl(215, 20%, 92%);
        --slate-200: hsl(215, 20%, 82%);
        --slate-300: hsl(215, 20%, 68%);
        --slate-400: hsl(215, 20%, 54%);
        --slate-500: hsl(215, 20%, 40%);
        --slate-600: hsl(215, 20%, 30%);
        --slate-700: hsl(215, 20%, 22%);
        --slate-800: hsl(215, 20%, 15%);
        --slate-900: hsl(215, 20%, 9%);   /* outer gutter behind cards */

        /* ── Orange ──────────────────────────────────────────────────────────
           H: 28  S: 90%
           Warm amber-orange. Used for warning text and icon tints.
           100 → pale peach   900 → deep burnt orange
        ──────────────────────────────────────────────────────────────────── */
        --orange-100: hsl(28, 90%, 94%);
        --orange-200: hsl(28, 88%, 82%);
        --orange-300: hsl(28, 85%, 68%);
        --orange-400: hsl(28, 82%, 55%);
        --orange-500: hsl(28, 80%, 44%);   /* readable dark-orange on dark bg */
        --orange-600: hsl(28, 78%, 35%);
        --orange-700: hsl(28, 75%, 26%);
        --orange-800: hsl(28, 72%, 18%);
        --orange-900: hsl(28, 70%, 12%);

        /* ── Teal ─────────────────────────────────────────────────────────────
           H: 180  S: 36%
           Midpoint between navy (205°) and green (155°).
           Token field borders, SELECT TOKEN surfaces, focus rings.
        ──────────────────────────────────────────────────────────────────── */
        --teal-100: hsl(180, 36%, 90%);
        --teal-200: hsl(180, 36%, 78%);
        --teal-300: hsl(180, 36%, 62%);
        --teal-400: hsl(180, 36%, 48%);
        --teal-500: hsl(180, 36%, 36%);
        --teal-600: hsl(180, 36%, 28%);
        --teal-700: hsl(180, 36%, 20%);   /* SELECT TOKEN box bg */
        --teal-800: hsl(180, 36%, 14%);
        --teal-900: hsl(180, 36%, 9%);
    }
`

export default styles
