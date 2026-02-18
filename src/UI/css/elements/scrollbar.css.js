import { css } from "/core/UI.js"

export function rules(selector = "*") {
    return `
    ${selector} {
        scrollbar-width: thin;
        scrollbar-color: var(--scrollbar-thumb) var(--scrollbar-track);
    }

    ${selector}::-webkit-scrollbar {
        width: var(--scrollbar-size);
        height: var(--scrollbar-size);
        border-radius: 0;
    }

    ${selector}::-webkit-scrollbar-track {
        background: var(--scrollbar-track);
        border-radius: 0;
    }

    ${selector}::-webkit-scrollbar-thumb {
        background: var(--scrollbar-thumb);
        border-radius: 0;
        border: var(--border-width) solid var(--scrollbar-track);
    }

    ${selector}::-webkit-scrollbar-thumb:vertical,
    ${selector}::-webkit-scrollbar-thumb:horizontal,
    ${selector}::-webkit-scrollbar-track:vertical,
    ${selector}::-webkit-scrollbar-track:horizontal {
        border-radius: 0;
    }

    ${selector}::-webkit-scrollbar-thumb:hover {
        background: var(--scrollbar-thumb-hover);
    }

    ${selector}::-webkit-scrollbar-corner {
        background: var(--scrollbar-corner);
    }
`
}

export const styles = css`
    ${rules("*")}
`

export default styles
