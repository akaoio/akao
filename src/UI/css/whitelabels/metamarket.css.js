import { css } from "/core/UI.js"

export const styles = css`
    /* Scoped to metamarket whitelabel only */
    :root[data-whitelabel="metamarket"] {
        /* MetaForge-inspired palette */
        --mm-orange: #e07b0a;   /* primary accent — premium, active, fire */
        --mm-gold:   #c9900a;   /* legendary, highlights */
        --mm-green:  #56a84a;   /* seeds, reputation, WTS/WTB badges */
        --mm-blue:   #4a8fff;   /* common/rare blueprint item names */
        --mm-red:    #cc3a3a;   /* negative rep, warnings */
        --mm-purple: #9b59b6;   /* epic rarity */

        /* Rarity colors */
        --rarity-legendary: #c9900a;
        --rarity-epic:      #e07b0a;
        --rarity-rare:      #4a8fff;
        --rarity-uncommon:  #56a84a;
        --rarity-common:    #888070;

        /* Glow shadows */
        --glow-orange: 0 0 8px #e07b0a88, 0 0 24px #e07b0a33;
        --glow-green:  0 0 8px #56a84a88, 0 0 24px #56a84a33;
        --glow-blue:   0 0 8px #4a8fff88, 0 0 24px #4a8fff33;

        /* Borders — warm, very subtle */
        --border-width:          1px;
        --border-accent:         var(--border-width) solid var(--mm-orange);
        --border-inverted:       var(--border-width) solid var(--mm-green);
        --border-accent-inset:   inset 0 0 0 var(--border-width) var(--mm-orange);
        --border-inverted-inset: inset 0 0 0 var(--border-width) var(--mm-green);

        /* Typography — Poppins */
        --font:         "Poppins", system-ui, sans-serif;
        --header-font:  "Poppins", system-ui, sans-serif;
        --content-font: "Poppins", system-ui, sans-serif;

        /* Layout hooks */
        --header-backdrop:   blur(8px);
        --footer-backdrop:   none;

        /* Rounded corners — 8px general, pill for badges/tags */
        --radius: 8px;
        --radius-pill: 9999px;

        /* Hero typography */
        --hero-title-shadow:   0 2px 20px rgba(224, 123, 10, 0.4), 0 0 60px rgba(224, 123, 10, 0.15);
        --hero-eyebrow-shadow: var(--glow-green);

        /* Section headers */
        --section-label-color:  var(--mm-orange);
        --section-label-shadow: var(--glow-orange);

        /* Item cards */
        --item-border-left:  3px solid var(--mm-orange);
        --item-name-color:   var(--mm-blue);
        --item-name-shadow:  none;
    }

    /* Dark variant — warm near-black, orange as primary fire */
    :root[data-whitelabel="metamarket"][data-theme="dark"] {
        --background:          #0f0d0a;
        --background-accent:   #161310;
        --background-focus:    rgba(224, 123, 10, 0.07);
        --background-inverted: #e0d5c8;
        --color:               #d8cfc4;
        --color-accent:        #e07b0a;
        --color-inverted:      #0f0d0a;

        --border:       var(--border-width) solid #252018;
        --border-inset: inset 0 0 0 var(--border-width) #252018;

        --whitelabel-background: var(--background);
        --whitelabel-color:      var(--mm-orange);

        --header-background:    rgba(15, 13, 10, 0.92);
        --header-border-bottom: var(--border-width) solid #2a2318;
        --footer-background:    #0c0a08;
        --footer-border-top:    var(--border-width) solid #252018;

        --scrollbar-track:       #0c0a08;
        --scrollbar-thumb:       #2a2318;
        --scrollbar-thumb-hover: var(--mm-orange);
        --scrollbar-corner:      #0c0a08;

        --section-line:    linear-gradient(to right, var(--mm-orange) 0%, #2a2318 40%, transparent 100%);
        --item-background: #131109;
        --item-hover-shadow: 0 0 0 1px #2a2318, 0 2px 12px rgba(224, 123, 10, 0.08);
    }

    /* Light variant — warm parchment, muted amber accents */
    :root[data-whitelabel="metamarket"][data-theme="light"] {
        --background:          #f5f0e8;
        --background-accent:   #ede5d5;
        --background-focus:    rgba(224, 123, 10, 0.08);
        --background-inverted: #0f0d0a;
        --color:               #1a1510;
        --color-accent:        #c06a08;
        --color-inverted:      #f5f0e8;

        --border:       var(--border-width) solid #c8b898;
        --border-inset: inset 0 0 0 var(--border-width) #c8b898;

        --whitelabel-background: var(--background);
        --whitelabel-color:      #c06a08;

        --header-background:    rgba(245, 240, 232, 0.95);
        --header-border-bottom: var(--border-width) solid #c8b898;
        --footer-background:    #ede5d5;
        --footer-border-top:    var(--border-width) solid #c8b898;

        --scrollbar-track:       #f5f0e8;
        --scrollbar-thumb:       #c8b898;
        --scrollbar-thumb-hover: #c06a08;
        --scrollbar-corner:      #f5f0e8;

        --section-line:    linear-gradient(to right, #c06a08 0%, #c8b898 40%, transparent 100%);
        --item-background: #ede5d5;
        --item-hover-shadow: 0 0 0 1px #c8b898, 0 2px 8px rgba(192, 106, 8, 0.08);
    }

    /* Body — no scanlines, minimal cursor override */
    :root[data-whitelabel="metamarket"] body {
        cursor: default;
    }

    /* Thin warm overlay to unify the dark bg */
    :root[data-whitelabel="metamarket"][data-theme="dark"] body::after {
        content: '';
        position: fixed;
        inset: 0;
        background: radial-gradient(ellipse at 50% 0%, rgba(224, 123, 10, 0.03) 0%, transparent 60%);
        pointer-events: none;
        z-index: 9999;
    }
`

export default styles
