import { css } from "/core/UI.js"

export const styles = css`
    .story-preview {
        /* ── Layout containers ────────────────────────────────────── */
        .typo-grid-4 {
            display: grid;
            grid-template-columns: repeat(4, 1fr);
            gap: 1.5rem;
            padding: 0.5rem 0;
        }

        .typo-grid-2 {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 2rem;
            padding: 0.5rem 0;
        }

        .typo-stack {
            display: flex;
            flex-direction: column;
            gap: 1.5rem;
            padding: 0.5rem 0;
        }

        .typo-stack--fw {
            display: flex;
            flex-direction: column;
            gap: 2rem;
            padding: 0.5rem 0;
        }

        .typo-stack--md {
            display: flex;
            flex-direction: column;
            gap: 1rem;
            padding: 0.5rem 0;
        }

        .typo-stack--sm {
            display: flex;
            flex-direction: column;
            gap: 0.75rem;
            padding: 0.5rem 0;
        }

        .typo-stack--xs {
            display: flex;
            flex-direction: column;
            gap: 0.5rem;
        }

        .typo-stack--ls {
            display: flex;
            flex-direction: column;
            gap: 1.25rem;
            padding: 0.5rem 0;
        }

        .typo-row {
            display: flex;
            align-items: center;
            gap: 1.5rem;
        }

        .typo-row--baseline {
            display: flex;
            align-items: baseline;
            gap: 1.5rem;
        }

        .typo-row--sm {
            display: flex;
            align-items: baseline;
            gap: 1rem;
        }

        /* ── Text utilities ───────────────────────────────────────── */
        .typo-meta {
            font-family: "Exo 2", sans-serif;
            font-size: 0.7rem;
            text-transform: uppercase;
            letter-spacing: 0.08em;
            opacity: 0.4;
        }

        .typo-meta--wide {
            font-family: "Exo 2", sans-serif;
            font-size: 0.7rem;
            text-transform: uppercase;
            letter-spacing: 0.1em;
            opacity: 0.5;
        }

        .typo-meta-val {
            font-family: "Exo 2", sans-serif;
            font-size: 0.7rem;
            opacity: 0.35;
        }

        .typo-meta-col {
            min-width: 10rem;
            display: flex;
            flex-direction: column;
            gap: 0.1rem;
        }

        .typo-meta-tag {
            font-family: "Exo 2", sans-serif;
            font-size: 0.7rem;
            text-transform: uppercase;
            letter-spacing: 0.08em;
            opacity: 0.4;
            min-width: 2.5rem;
        }

        .typo-meta-tag--sm {
            font-family: "Exo 2", sans-serif;
            font-size: 0.7rem;
            opacity: 0.4;
            min-width: 2rem;
        }

        .typo-meta-tag--ls {
            min-width: 5rem;
        }

        .typo-body {
            font-family: var(--body-font), sans-serif;
        }

        .typo-header {
            font-family: var(--header-font), sans-serif;
        }

        .typo-dim {
            opacity: 0.6;
        }

        .typo-mb-sm { margin-bottom: 0.5rem; }
        .typo-mb-md { margin-bottom: 0.75rem; }
        .typo-mb-lg { margin-bottom: 1rem; }

        /* ── Neon Colors ──────────────────────────────────────────── */
        .typo-swatch {
            height: var(--space-6);
            margin-bottom: 0.75rem;
        }

        .typo-swatch--g { background: var(--neon-g); box-shadow: var(--glow-g); }
        .typo-swatch--c { background: var(--neon-c); box-shadow: var(--glow-c); }
        .typo-swatch--m { background: var(--neon-m); box-shadow: var(--glow-m); }
        .typo-swatch--y { background: var(--neon-y); box-shadow: var(--glow-y); }

        .typo-swatch-label {
            font-family: "Exo 2", sans-serif;
            font-size: 0.75rem;
            text-transform: uppercase;
            letter-spacing: 0.08em;
            opacity: 0.6;
            margin-bottom: 0.25rem;
        }

        .typo-hex {
            font-family: "Orbitron", sans-serif;
            font-size: 1.5rem;
            font-weight: 700;
        }

        .typo-hex--g { color: var(--neon-g); text-shadow: var(--glow-g); }
        .typo-hex--c { color: var(--neon-c); text-shadow: var(--glow-c); }
        .typo-hex--m { color: var(--neon-m); text-shadow: var(--glow-m); }
        .typo-hex--y { color: var(--neon-y); text-shadow: var(--glow-y); }

        /* ── Neon Glow Effects ────────────────────────────────────── */
        .typo-glow-label {
            font-family: "Exo 2", sans-serif;
            font-size: 0.7rem;
            text-transform: uppercase;
            letter-spacing: 0.1em;
            opacity: 0.5;
            margin-bottom: 0.5rem;
        }

        .typo-glow-demo {
            font-family: "Orbitron", sans-serif;
            font-size: 2.5rem;
            font-weight: 700;
            letter-spacing: 0.05em;
        }

        .typo-glow-demo--g { color: var(--neon-g); text-shadow: var(--glow-g); }
        .typo-glow-demo--c { color: var(--neon-c); text-shadow: var(--glow-c); }
        .typo-glow-demo--m { color: var(--neon-m); text-shadow: var(--glow-m); }
        .typo-glow-demo--y { color: var(--neon-y); text-shadow: var(--glow-y); }

        /* ── Heading Scale ────────────────────────────────────────── */
        .typo-h1 {
            font-family: "Orbitron", sans-serif;
            font-size: 3rem;
            font-weight: 700;
            letter-spacing: 0.04em;
            color: var(--neon-c);
            text-shadow: var(--glow-c);
            line-height: 1.1;
            margin: 0;
        }

        .typo-h2 {
            font-family: "Orbitron", sans-serif;
            font-size: 2.25rem;
            font-weight: 700;
            letter-spacing: 0.04em;
            color: var(--neon-c);
            text-shadow: var(--glow-c);
            line-height: 1.15;
            margin: 0;
        }

        .typo-h3 {
            font-family: "Orbitron", sans-serif;
            font-size: 1.75rem;
            font-weight: 600;
            letter-spacing: 0.03em;
            color: var(--neon-g);
            text-shadow: var(--glow-g);
            line-height: 1.2;
            margin: 0;
        }

        .typo-h4 {
            font-family: "Orbitron", sans-serif;
            font-size: 1.25rem;
            font-weight: 600;
            letter-spacing: 0.03em;
            line-height: 1.25;
            margin: 0;
        }

        .typo-h5 {
            font-family: "Orbitron", sans-serif;
            font-size: 1rem;
            font-weight: 600;
            letter-spacing: 0.03em;
            line-height: 1.3;
            margin: 0;
        }

        .typo-h6 {
            font-family: "Orbitron", sans-serif;
            font-size: 0.875rem;
            font-weight: 600;
            letter-spacing: 0.05em;
            text-transform: uppercase;
            opacity: 0.75;
            line-height: 1.4;
            margin: 0;
        }

        /* ── Font Size Scale ──────────────────────────────────────── */
        .typo-size-xs { font-family: "Exo 2", sans-serif; font-size: var(--text-xs); }
        .typo-size-sm { font-family: "Exo 2", sans-serif; font-size: var(--text-sm); }
        .typo-size-md { font-family: "Exo 2", sans-serif; font-size: var(--text-md); }
        .typo-size-lg { font-family: "Exo 2", sans-serif; font-size: var(--text-lg); }
        .typo-size-xl { font-family: "Exo 2", sans-serif; font-size: var(--text-xl); }

        .typo-size-display {
            font-family: "Orbitron", sans-serif;
            font-size: 2rem;
            font-weight: 700;
        }

        .typo-size-hero {
            font-family: "Orbitron", sans-serif;
            font-size: 3rem;
            font-weight: 700;
            color: var(--neon-c);
            text-shadow: var(--glow-c);
        }

        /* ── Font Families ────────────────────────────────────────── */
        .typo-ff-orbitron {
            font-family: "Orbitron", sans-serif;
            font-size: 1.75rem;
            font-weight: 700;
            letter-spacing: 0.04em;
        }

        .typo-ff-exo2 {
            font-family: "Exo 2", sans-serif;
            font-size: 1.5rem;
            font-weight: 400;
        }

        .typo-ff-noto {
            font-family: "Noto Sans", sans-serif;
            font-size: 1.5rem;
            font-weight: 400;
        }

        .typo-ff-poppins {
            font-family: "Poppins", sans-serif;
            font-size: 1.5rem;
            font-weight: 400;
        }

        /* ── Font Weights ─────────────────────────────────────────── */
        .typo-fw-sample { font-size: 1.1rem; }

        .typo-w-100 { font-weight: 100; }
        .typo-w-200 { font-weight: 200; }
        .typo-w-300 { font-weight: 300; }
        .typo-w-400 { font-weight: 400; }
        .typo-w-500 { font-weight: 500; }
        .typo-w-600 { font-weight: 600; }
        .typo-w-700 { font-weight: 700; }
        .typo-w-800 { font-weight: 800; }
        .typo-w-900 { font-weight: 900; }

        /* ── Letter Spacing ───────────────────────────────────────── */
        .typo-ls-sample {
            font-family: "Orbitron", sans-serif;
            font-size: 1.25rem;
            font-weight: 600;
        }

        .typo-ls-tight   { letter-spacing: -0.05em; }
        .typo-ls-normal  { letter-spacing: 0; }
        .typo-ls-wide    { letter-spacing: 0.05em; }
        .typo-ls-wider   { letter-spacing: 0.1em; }
        .typo-ls-widest  { letter-spacing: 0.2em; }
        .typo-ls-ultra   { letter-spacing: 0.4em; }
    }
`

export default styles
