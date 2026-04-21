import { css } from "/core/UI.js"
import { mq } from "/UI/css/breakpoints.js"

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
        --space: var(--space-3);
        --space-sm: var(--space-2);
        --space-md: var(--space-3);
        --space-lg: var(--space-4);

        /* Typography — fluid via clamp(min, viewport, max).
           Max values match the previous fixed sizes (0.125rem × multiplier).
           Min values keep text legible on small phones. */
        --text-xs: clamp(0.65rem, 1.5vw, 0.75rem);
        --text-sm: clamp(0.75rem, 1.8vw, 0.875rem);
        --text-md: clamp(0.875rem, 2vw, 1rem);
        --text-lg: clamp(1rem, 2.5vw, 1.25rem);
        --text-xl: clamp(1.25rem, 3vw, 1.5rem);
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
        --icon-xlg: var(--unit-10);
        --icon-2xl: var(--unit-9); /* 6rem = 96px */
        --icon: var(--icon-md);

        /* Icon Masks — square viewBox, default direction left; rotate() to repoint */
        --icon-chevron-double: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 16 16'%3E%3Cpath d='M7 11 L4 8 L7 5 M12 11 L9 8 L12 5' fill='none' stroke='white' stroke-width='1.5' stroke-linecap='round' stroke-linejoin='round'/%3E%3C/svg%3E");

        /* Layout */
        --max-width: 80rem;
        --content-width: min(100% - var(--space-4), var(--max-width));
        --box-width: calc((min(100vw, 100vh) - var(--space-4)) * 0.75);

        --container-sm: 33.75rem; /* 540px  — phone landscape cap */
        --container-md: 45rem; /* 720px  — tablet cap          */
        --container-lg: 60rem; /* 960px  — desktop cap         */
        --container-xl: 71.25rem; /* 1140px — wide desktop cap    */
        --container-xxl: 82.5rem; /* 1320px — ultra-wide cap      */
        --container-form: 30rem; /* 480px  — narrow form/card    */

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

        /* ── Semantic Surface Tokens ──────────────────────────────────────────
           Abstracts palette stops into role-based names.
           Components reference these; never reference --navy-*, --green-*, etc. directly.
        ──────────────────────────────────────────────────────────────────── */

        /* Card surfaces */
        --card-bg: var(--navy-800);
        --card-header-bg: color-mix(in hsl, var(--green-900) 60%, var(--navy-900));
        --card-header-border: color-mix(in hsl, var(--green-700) 30%, var(--navy-700));
        --card-shadow: 0 6px 40px color-mix(in hsl, var(--neon-c) 10%, transparent);

        /* Card toolbar band (mirrors .catalog-toolbar anatomy) */
        --card-toolbar-bg: color-mix(in hsl, var(--color) 3%, transparent);
        --card-toolbar-border: color-mix(in hsl, var(--color) 12%, transparent);
        --card-toolbar-bg-focus: color-mix(in hsl, var(--neon-c) 3%, transparent);
        --card-toolbar-border-focus: color-mix(in hsl, var(--neon-c) 45%, transparent);

        /* Field / token input surfaces (replaces raw --navy-700 / --teal-700 usage) */
        --field-bg: color-mix(in hsl, var(--navy-700) 35%, transparent);
        --field-bg-hover: color-mix(in hsl, var(--teal-700) 25%, transparent);
        --field-bg-focus: color-mix(in hsl, var(--teal-700) 40%, transparent);
        --field-inset-bg: color-mix(in hsl, var(--navy-700) 25%, transparent);
        --field-inset-border: color-mix(in hsl, var(--navy-700) 60%, transparent);

        /* Semantic interactive accents */
        --accent-info: var(--neon-c); /* filters, search, links, info states */
        --accent-action: var(--neon-g); /* confirm, submit, price, positive */
        --accent-success: var(--neon-g); /* success states, completed actions */
        --accent-warning: var(--neon-y); /* caution, irreversible actions, pre-error */
        --accent-danger: var(--neon-m); /* error, destructive, blocked */
        --glow-info: var(--glow-c);
        --glow-action: var(--glow-g);
        --glow-success: var(--glow-g);
        --glow-warning: var(--glow-y);
        --glow-danger: var(--glow-m);

        /* Interactive surface tokens — resting border, glow shadow, hover/selected fill */
        --accent-info-border: color-mix(in hsl, var(--accent-info) 35%, transparent);
        --accent-info-glow: color-mix(in hsl, var(--accent-info) 12%, transparent);
        --accent-info-surface: color-mix(in hsl, var(--accent-info) 8%, transparent);
        --accent-action-border: color-mix(in hsl, var(--accent-action) 50%, transparent);
        --accent-action-glow: color-mix(in hsl, var(--accent-action) 20%, transparent);
        --accent-action-surface: color-mix(in hsl, var(--accent-action) 12%, transparent);
        --accent-success-border: color-mix(in hsl, var(--accent-success) 50%, transparent);
        --accent-success-glow: color-mix(in hsl, var(--accent-success) 20%, transparent);
        --accent-success-surface: color-mix(in hsl, var(--accent-success) 12%, transparent);
        --accent-warning-border: color-mix(in hsl, var(--accent-warning) 40%, transparent);
        --accent-warning-glow: color-mix(in hsl, var(--accent-warning) 15%, transparent);
        --accent-warning-surface: color-mix(in hsl, var(--accent-warning) 10%, transparent);
        --accent-danger-border: color-mix(in hsl, var(--accent-danger) 35%, transparent);
        --accent-danger-glow: color-mix(in hsl, var(--accent-danger) 12%, transparent);
        --accent-danger-surface: color-mix(in hsl, var(--accent-danger) 8%, transparent);

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
        --rarity-epic: var(--rarity-magic);
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

        /* Z-index Layers */
        --z-base: 0;
        --z-sticky: 50; /* sticky catalog bands             */
        --z-dropdown: 100; /* dropdowns, navigator             */
        --z-panel-toggle: 200;
        --z-panel-backdrop: 210;
        --z-panel: 220;
        --z-modal-backdrop: 290; /* sits just below modal            */
        --z-modal: 300;
        --z-overlay: 400;
        --z-toast: 500;
        --z-picker-backdrop: 590; /* sits just below picker           */
        --z-picker: 600; /* avatar picker, floating pickers  */
        --z-devtools: 999; /* internal dev tooling only        */

        /* Blur Scale */
        --blur-xs: 2px;
        --blur-sm: 4px;
        --blur-md: 8px;
        --blur-lg: 12px;
        --blur-xl: 1rem;

        /* Structural Shadows */
        --shadow-sm: 0 1px 3px color-mix(in hsl, var(--color) 8%, transparent);
        --shadow-md: 0 4px 16px color-mix(in hsl, var(--color) 10%, transparent);
        --shadow-lg: 0 8px 32px color-mix(in hsl, var(--color) 12%, transparent);

        /* Component Sizing */
        --panel-width: min(20rem, 85vw);
        --logo-width: 4rem;
        --logo-height: 2.25rem;
        --brand-max-width: 8rem;
        --progress-width: 10rem;
        --progress-height: var(--space-1);

        /* Hero layout */
        --hero-pad-top: clamp(5rem, 13vw, 9rem);
        --avatar-size: clamp(5rem, 12vw, 8rem);
    }

    @media ${mq.mdDown} {
        :root {
            --hero-pad-top: clamp(4rem, 11vw, 7rem);
        }
    }

    @media ${mq.smDown} {
        :root {
            --hero-pad-top: clamp(3.5rem, 10vw, 6rem);
        }
    }

    @media ${mq.xsDown} {
        :root {
            --hero-pad-top: clamp(3rem, 9vw, 4.5rem);
            --avatar-size: clamp(4rem, 10vw, 5.5rem);
        }
    }

    * {
        margin: 0;
        padding: 0;
        box-sizing: border-box;
    }
`

export default styles
