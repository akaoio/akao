import { css } from "/core/UI.js"

export const styles = css`
    /* Default whitelabel — clean, neutral, inherits base vars */

    :root[data-whitelabel="default"][data-theme="dark"] {
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
    }

    :root[data-whitelabel="default"][data-theme="light"] {
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
    }
`

export default styles
