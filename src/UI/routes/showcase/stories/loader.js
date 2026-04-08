import "/UI/components/loader/index.js"

const constrain = (el) => el.style.setProperty("--loader-min-height", "0")

export default {
    name: "Loader",
    stories: [
        {
            name: "Default",
            code: `<ui-loader></ui-loader>`,
            setup: constrain
        },
        {
            name: "Custom color",
            code: `<ui-loader style="--loader-color: var(--neon-m)"></ui-loader>`,
            setup: constrain
        }
    ]
}
