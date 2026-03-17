import { css } from "/core/UI.js"

/**
 * Whitelabel Template — copy this file to `yourname.css.js` and fill in your values.
 *
 * ONBOARDING STEPS:
 *   1. Copy this file to `src/UI/css/whitelabels/yourname.css.js`
 *   2. Import it in `src/UI/css/global.css.js`:
 *        import yourname from "./whitelabels/yourname.css.js"
 *        ...and add ${yourname} to the template literal (after ${dark})
 *   3. Add your entry to `src/statics/whitelabels.yaml`:
 *        - code: yourname
 *      Put it first to make it the default.
 *
 * CUSTOM FONTS:
 *   Add a @font-face or @import at the top of your whitelabel block, or extend
 *   `src/UI/css/externals.css.js` with a Google Fonts URL for your typefaces.
 *
 * OPTIONAL vs REQUIRED:
 *   Every variable listed here has a fallback — removing a variable is safe.
 *   The fallback value is shown in the comment after each line.
 *
 * SCOPING:
 *   All rules MUST be scoped to [data-whitelabel="yourname"] to avoid bleeding
 *   into other whitelabels when multiple are bundled.
 */

export const styles = css`

    /* ─── Shared palette & typography ───────────────────────────────────────── */
    /* Remove this block if you have no theme-agnostic overrides.                */

    :root[data-whitelabel="yourname"] {

        /* Typography — fallbacks: "Noto Sans", system-ui, sans-serif */
        --font:         "Your Font", system-ui, sans-serif;
        --header-font:  "Your Font", system-ui, sans-serif;
        --content-font: "Your Font", system-ui, sans-serif;

        /* Corner radius — fallback: 0 (flat) */
        --radius: 0;

        /* Header & footer glass effect — fallback: none (opaque) */
        --header-backdrop: blur(12px);
        --footer-backdrop: blur(12px);

        /* Hero section typography shadows — fallback: none */
        --hero-title-shadow:   0 0 30px rgba(0,0,0,0.2);
        --hero-eyebrow-shadow: none;

        /* Section header — fallback: var(--color), none */
        --section-label-color:  var(--color-accent);
        --section-label-shadow: none;

        /* Item cards — fallbacks: var(--border), var(--color-accent), none */
        --item-border-left: var(--border);
        --item-name-color:  var(--color-accent);
        --item-name-shadow: none;
    }


    /* ─── Dark theme ─────────────────────────────────────────────────────────── */

    :root[data-whitelabel="yourname"][data-theme="dark"] {

        /* Core palette — override to replace the base HSL color system entirely */
        /* --background:          #0d0d0d; */
        /* --background-accent:   #1a1a1a; */
        /* --background-focus:    rgba(255,255,255,0.06); */
        /* --background-inverted: #f0f0f0; */
        /* --color:               #f0f0f0; */
        /* --color-accent:        #your-brand-color; */
        /* --color-inverted:      #0d0d0d; */

        /* Borders — fallback: computed from --color via color-mix */
        /* --border:       1px solid #2a2a2a; */
        /* --border-inset: inset 0 0 0 1px #2a2a2a; */

        /* Brand identity variables — consumed by header .brand and other components */
        --whitelabel-background: var(--background);   /* fallback: var(--background) */
        --whitelabel-color:      var(--color-accent);  /* fallback: var(--color) */

        /* Header chrome */
        --header-background:    var(--background);    /* fallback: var(--background) */
        --header-border-bottom: var(--border);        /* fallback: none */

        /* Footer chrome */
        --footer-background:    var(--background);    /* fallback: var(--background) */
        --footer-border-top:    var(--border);        /* fallback: none */

        /* Scrollbar */
        --scrollbar-track:       var(--background);
        --scrollbar-thumb:       color-mix(in hsl, var(--color) 40%, transparent);
        --scrollbar-thumb-hover: var(--color-accent);
        --scrollbar-corner:      var(--background);

        /* Section decorative line — fallback: var(--color) */
        --section-line: linear-gradient(to right, var(--color-accent) 0%, transparent 100%);

        /* Item card backgrounds — fallback: transparent, var(--border-inset) */
        --item-background:   var(--background-accent);
        --item-hover-shadow: var(--border-inset);
    }


    /* ─── Light theme ────────────────────────────────────────────────────────── */

    :root[data-whitelabel="yourname"][data-theme="light"] {

        /* --background:          #f8f8f8; */
        /* --background-accent:   #efefef; */
        /* --color:               #111111; */
        /* --color-accent:        #your-brand-color; */

        --whitelabel-background: var(--background);
        --whitelabel-color:      var(--color-accent);

        --header-background:    var(--background);
        --header-border-bottom: var(--border);

        --footer-background:    var(--background);
        --footer-border-top:    var(--border);

        --scrollbar-track:       var(--background);
        --scrollbar-thumb:       color-mix(in hsl, var(--color) 40%, transparent);
        --scrollbar-thumb-hover: var(--color-accent);
        --scrollbar-corner:      var(--background);

        --section-line:  linear-gradient(to right, var(--color-accent) 0%, transparent 100%);
        --item-background:   var(--background-accent);
        --item-hover-shadow: var(--border-inset);
    }


    /* ─── Body effects (optional) ────────────────────────────────────────────── */
    /* Remove this block if you don't need custom cursor or overlays.             */

    /* :root[data-whitelabel="yourname"] body {
        cursor: default;
    } */

`

export default styles
