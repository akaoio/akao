import { css } from "/core/UI.js"

export const styles = css`
    :root[data-theme="light"],
    .light {
        --l1: 100%;
        --l1-inverted: 0%;

        /* Same hues as dark, at +15pp lightness vs previous */
        --neon-g: #00d483;
        --neon-c: #00b3c7;
        --neon-m: #e13576;
        --neon-y: #e7ab00;

        /* Glow — updated to match brightened neons */
        --glow-g: 0 0 8px rgba(0, 212, 131, 0.3),  0 0 20px rgba(0, 212, 131, 0.1);
        --glow-c: 0 0 8px rgba(0, 179, 199, 0.3),  0 0 20px rgba(0, 179, 199, 0.1);
        --glow-m: 0 0 8px rgba(225, 53, 118, 0.3), 0 0 20px rgba(225, 53, 118, 0.1);
        --glow-y: 0 0 8px rgba(231, 171, 0, 0.3),  0 0 20px rgba(231, 171, 0, 0.1);

        /* Apple-style surfaces: off-white bg, pure white cards */
        --background:          #f5f5f7;
        --background-accent:   #ffffff;
        --background-focus:    rgba(0, 110, 122, 0.08);
        --background-inverted: #1d1d1f;
        --color:               #444446;
        --color-accent:        #00d483;
        --color-inverted:      #f5f5f7;

        /* Apple gossamer dividers — near-invisible */
        --border:       var(--border-width) solid rgba(0, 0, 0, 0.08);
        --border-inset: inset 0 0 0 var(--border-width) rgba(0, 0, 0, 0.08);

        /* Hero shadow */
        --hero-title-shadow: 0 0 30px rgba(0, 110, 122, 0.2), 0 0 80px rgba(0, 110, 122, 0.08);

        --whitelabel-color: var(--neon-c);

        /* Apple frosted-glass header */
        --header-background:    rgba(255, 255, 255, 0.72);
        --header-border-bottom: var(--border-width) solid rgba(0, 0, 0, 0.08);
        --footer-background:    rgba(245, 245, 247, 0.9);
        --footer-border-top:    var(--border-width) solid var(--neon-g);

        /* Apple-style near-invisible scrollbar */
        --scrollbar-track:       transparent;
        --scrollbar-thumb:       rgba(0, 0, 0, 0.15);
        --scrollbar-thumb-hover: var(--neon-c);
        --scrollbar-corner:      transparent;

        --section-line:      linear-gradient(to right, var(--neon-g) 0%, rgba(0, 0, 0, 0.08) 40%, transparent 100%);
        /* White cards on gray bg — Apple's depth trick */
        --item-background:   #ffffff;
        --item-hover-shadow: 0 4px 20px rgba(0, 0, 0, 0.08), 0 1px 4px rgba(0, 0, 0, 0.04);
    }
`

export default styles
