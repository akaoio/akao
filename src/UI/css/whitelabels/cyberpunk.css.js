import { css } from "/core/UI.js"

export const styles = css`
    /* Scoped to cyberpunk whitelabel only */
    :root[data-whitelabel="cyberpunk"] {
        /* Neon palette */
        --neon-g:  #00ff9d;
        --neon-c:  #00e5ff;
        --neon-m:  #ff2d78;
        --neon-y:  #f5e642;

        /* Glow shadows */
        --glow-g:  0 0 8px #00ff9d88, 0 0 24px #00ff9d33;
        --glow-c:  0 0 8px #00e5ff88, 0 0 24px #00e5ff33;
        --glow-m:  0 0 8px #ff2d7888, 0 0 24px #ff2d7833;
        --glow-y:  0 0 8px #f5e64288, 0 0 24px #f5e64233;

        /* Borders */
        --border-width:          1px;
        --border-accent:         var(--border-width) solid var(--neon-c);
        --border-inverted:       var(--border-width) solid var(--neon-g);
        --border-accent-inset:   inset 0 0 0 var(--border-width) var(--neon-c);
        --border-inverted-inset: inset 0 0 0 var(--border-width) var(--neon-g);

        /* Typography */
        --font:         "Exo 2", system-ui, sans-serif;
        --header-font:  "Orbitron", system-ui, sans-serif;
        --content-font: "Exo 2", system-ui, sans-serif;

        /* Layout hooks — consumed by header/footer */
        --header-backdrop:   blur(12px);
        --footer-backdrop:   blur(12px);

        /* Keep flat/sharp corners */
        --radius: 0;

        /* Hero typography */
        --hero-title-shadow:   0 0 30px #00e5ff44, 0 0 80px #00e5ff1a;
        --hero-eyebrow-shadow: var(--glow-g);

        /* Section headers */
        --section-label-color:  var(--neon-g);
        --section-label-shadow: var(--glow-g);

        /* Item cards */
        --item-border-left:  3px solid var(--neon-c);
        --item-name-color:   var(--neon-c);
        --item-name-shadow:  var(--glow-c);
    }

    /* Dark variant — near-black bg, full neon intensity */
    :root[data-whitelabel="cyberpunk"][data-theme="dark"] {
        --background:          #070b0f;
        --background-accent:   #0c1218;
        --background-focus:    rgba(0, 229, 255, 0.08);
        --background-inverted: #c8dde8;
        --color:               #c8dde8;
        --color-accent:        #00ff9d;
        --color-inverted:      #070b0f;

        --border:       var(--border-width) solid #1a2d3a;
        --border-inset: inset 0 0 0 var(--border-width) #1a2d3a;

        --whitelabel-background: var(--background);
        --whitelabel-color:      var(--neon-c);

        --header-background:    rgba(7, 11, 15, 0.85);
        --header-border-bottom: var(--border-width) solid var(--neon-c);
        --footer-background:    rgba(7, 11, 15, 0.85);
        --footer-border-top:    var(--border-width) solid var(--neon-g);

        --scrollbar-track:       var(--background);
        --scrollbar-thumb:       #1a2d3a;
        --scrollbar-thumb-hover: var(--neon-c);
        --scrollbar-corner:      var(--background);

        --section-line:    linear-gradient(to right, var(--neon-g) 0%, #1a2d3a 40%, transparent 100%);
        --item-background: #0c1218;
        --item-hover-shadow: 0 0 20px #00e5ff11, inset 0 0 30px #00e5ff06;
    }

    /* Light variant — bright surface, muted/accessible neon */
    :root[data-whitelabel="cyberpunk"][data-theme="light"] {
        --background:          #e8f4f8;
        --background-accent:   #d0e8f0;
        --background-focus:    rgba(0, 229, 255, 0.12);
        --background-inverted: #070b0f;
        --color:               #0a1a24;
        --color-accent:        #00b870;
        --color-inverted:      #e8f4f8;

        --border:       var(--border-width) solid #9ab8c8;
        --border-inset: inset 0 0 0 var(--border-width) #9ab8c8;

        --whitelabel-background: var(--background);
        --whitelabel-color:      #0080aa;

        --header-background:    rgba(232, 244, 248, 0.9);
        --header-border-bottom: var(--border-width) solid #0080aa;
        --footer-background:    rgba(232, 244, 248, 0.9);
        --footer-border-top:    var(--border-width) solid #00b870;

        --scrollbar-track:       var(--background);
        --scrollbar-thumb:       #9ab8c8;
        --scrollbar-thumb-hover: #0080aa;
        --scrollbar-corner:      var(--background);

        --section-line:    linear-gradient(to right, #00b870 0%, #9ab8c8 40%, transparent 100%);
        --item-background: #d0e8f0;
        --item-hover-shadow: 0 0 20px rgba(0, 128, 170, 0.1), inset 0 0 30px rgba(0, 128, 170, 0.04);
    }

    /* Body effects */
    :root[data-whitelabel="cyberpunk"] body {
        cursor: crosshair;
    }

    :root[data-whitelabel="cyberpunk"] body::after {
        content: '';
        position: fixed;
        inset: 0;
        background: repeating-linear-gradient(
            to bottom,
            transparent 0px,
            transparent 3px,
            rgba(0, 0, 0, 0.07) 3px,
            rgba(0, 0, 0, 0.07) 4px
        );
        pointer-events: none;
        z-index: 9999;
    }
`

export default styles
