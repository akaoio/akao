import { css } from "/core/UI.js"

export const styles = css`
    :root {
        /* Core Units */
        --unit-1: 0.075rem;
        --unit-2: 0.125rem;
        --unit-3: 0.25rem;
        --unit-4: 0.5rem;
        --unit-5: 1rem;
        --unit-6: 2rem;
        --unit-7: 3rem;
        --unit-8: 4rem;
        --unit-9: 6rem;
        --unit-10: 8rem;
        --unit: var(--unit-2);

        /* Spacing Scale */
        --space-0: 0;
        --space-1: calc(var(--unit) * 2);
        --space-2: calc(var(--unit) * 4);
        --space-3: calc(var(--unit) * 6);
        --space-4: calc(var(--unit) * 8);
        --space-5: calc(var(--unit) * 12);
        --space-6: calc(var(--unit) * 16);
        --space-7: calc(var(--unit) * 24);
        --space-8: calc(var(--unit) * 32);
        --space-9: calc(var(--unit) * 48);
        --space-10: calc(var(--unit) * 64);
        --space: var(--space-2);
        --space-sm: var(--space-1);
        --space-md: var(--space-2);
        --space-lg: var(--space-3);

        /* Typography */
        --text-xs: calc(var(--unit) * 6);
        --text-sm: calc(var(--unit) * 7);
        --text-md: calc(var(--unit) * 8);
        --text-lg: calc(var(--unit) * 10);
        --text-xl: calc(var(--unit) * 12);
        --text: var(--text-md);

        /* Animation */
        --speed-1: 0.125s;
        --speed-2: 0.25s;
        --speed-3: 0.5s;
        --speed-4: 1s;
        --speed-5: 2s;
        --speed: var(--speed-2);

        /* Theme Colors */
        --h1-1: calc(360 / 12);
        --h1-2: calc(360 / 12 * 2);
        --h1-3: calc(360 / 12 * 3);
        --h1-4: calc(360 / 12 * 4);
        --h1-5: calc(360 / 12 * 5);
        --h1-6: calc(360 / 12 * 6);
        --h1-7: calc(360 / 12 * 7);
        --h1-8: calc(360 / 12 * 8);
        --h1-9: calc(360 / 12 * 9);
        --h1-10: calc(360 / 12 * 10);
        --h1-11: calc(360 / 12 * 11);
        --h1-12: calc(360 / 12 * 12);
        --h1: var(--h1-1);
        --s1: 90%;
        --l1: 0%;

        /* Theme Accents */
        --h2-1: calc(360 / 12);
        --h2-2: calc(360 / 12 * 2);
        --h2-3: calc(360 / 12 * 3);
        --h2-4: calc(360 / 12 * 4);
        --h2-5: calc(360 / 12 * 5);
        --h2-6: calc(360 / 12 * 6);
        --h2-7: calc(360 / 12 * 7);
        --h2-8: calc(360 / 12 * 8);
        --h2-9: calc(360 / 12 * 9);
        --h2-10: calc(360 / 12 * 10);
        --h2-11: calc(360 / 12 * 11);
        --h2-12: calc(360 / 12 * 12);
        --h2: var(--h2-3);
        --s2: 70%;
        --l2: 60%;

        /* Color System */
        --background: hsl(var(--h1), var(--s1), var(--l1));
        --background-focus: color-mix(in hsl, var(--background-inverted) 10%, transparent 10%);
        --background-accent: hsl(var(--h2), var(--s2), var(--l2));
        --background-inverted: hsl(var(--h1), var(--s1), var(--l1-inverted));
        --color: hsl(var(--h1), var(--s1), var(--l1-inverted));
        --color-accent: hsl(var(--h2), var(--s2), var(--l2));
        --color-inverted: hsl(var(--h1), var(--s1), var(--l1));

        /* Borders */
        --border-width: 1px;
        --border: var(--border-width) solid color-mix(in hsl, var(--color) 75%, transparent 50%);
        --border-accent: var(--border-width) solid var(--neon-c);
        --border-inverted: var(--border-width) solid var(--neon-g);
        --border-inset: inset 0 0 0 var(--border-width) color-mix(in hsl, var(--color) 75%, transparent 50%);
        --border-accent-inset: inset 0 0 0 var(--border-width) var(--neon-c);
        --border-inverted-inset: inset 0 0 0 var(--border-width) var(--neon-g);

        /* Roundness */
        --radius-sm: var(--space);
        --radius-md: var(--space-2);
        --radius-lg: var(--space-3);
        --radius: 0;

        /* Icon Size */
        --icon-sm: var(--unit-5);
        --icon-md: var(--unit-6);
        --icon-lg: var(--unit-7);
        --icon: var(--icon-md);

        /* Layout */
        --max-width: 80rem;
        --content-width: min(100% - var(--space-4), var(--max-width));
        --box-width: calc((min(100vw, 100vh) - var(--space-4)) * 0.75);

        /* Font */
        --header-font: "Orbitron", system-ui, sans-serif;
        --content-font: "Exo 2", system-ui, sans-serif;
        --font: "Exo 2", system-ui, sans-serif;

        /* Neon palette */
        --neon-g: #00ff9d;
        --neon-c: #00e5ff;
        --neon-m: #ff2d78;
        --neon-y: #f5e642;

        /* Glow shadows */
        --glow-g: 0 0 8px #00ff9d88, 0 0 24px #00ff9d33;
        --glow-c: 0 0 8px #00e5ff88, 0 0 24px #00e5ff33;
        --glow-m: 0 0 8px #ff2d7888, 0 0 24px #ff2d7833;
        --glow-y: 0 0 8px #f5e64288, 0 0 24px #f5e64233;

        /* Layout hooks */
        --header-backdrop: blur(12px);
        --footer-backdrop: blur(12px);

        /* Hero typography */
        --hero-title-shadow: 0 0 30px #00e5ff44, 0 0 80px #00e5ff1a;
        --hero-eyebrow-shadow: var(--glow-g);

        /* Section headers */
        --section-label-color: var(--neon-g);
        --section-label-shadow: var(--glow-g);

        /* Item cards */
        --item-border-left: 3px solid var(--neon-c);
        --item-name-color: var(--neon-c);
        --item-name-shadow: var(--glow-c);

        /* Rarity palette */
        --rarity-legendary: var(--neon-y);
        --rarity-unique: var(--neon-m);
        --rarity-rare: var(--neon-c);
        --rarity-magic: hsl(270, 70%, 68%);
        --rarity-common: color-mix(in hsl, var(--color) 45%, transparent);
        --rarity-special: var(--neon-g);
        --rarity-normal: color-mix(in hsl, var(--color) 60%, transparent);
        --rarity-uncommon: color-mix(in hsl, var(--neon-g) 60%, var(--color));
        --rarity-epic: hsl(270, 70%, 68%);
        --rarity-mythic: var(--neon-g);

        --glow-legendary: var(--glow-y);
        --glow-unique: var(--glow-m);
        --glow-rare: var(--glow-c);

        /* Scrollbars */
        --scrollbar-size: var(--unit-4);
        --scrollbar-track: var(--background);
        --scrollbar-thumb: var(--color);
        --scrollbar-thumb-hover: var(--color-accent);
        --scrollbar-corner: var(--background);
    }

    * {
        margin: 0;
        padding: 0;
        box-sizing: border-box;
    }
`

export default styles
