import "/UI/components/svg/index.js"

export default {
    name: "Logo",
    stories: [
        {
            name: "Brand text",
            code: `<ui-svg data-src="/statics/sites/localhost/images/text.svg" class="svg-lg svg-color"></ui-svg>`
        },
        {
            name: "Brand symbol",
            code: `<ui-svg data-src="/statics/sites/localhost/images/symbol.svg" class="svg-md svg-color"></ui-svg>`
        },
        {
            name: "Brand symbol — accent",
            code: `<ui-svg data-src="/statics/sites/localhost/images/symbol.svg" class="svg-md svg-color-accent"></ui-svg>`
        },
        {
            name: "Favicon",
            code: `<ui-svg data-src="/statics/sites/localhost/images/favicon.svg" class="svg-md svg-color"></ui-svg>`
        },
        {
            name: "Symbol + Text (lockup)",
            code: `<div style="display:flex;align-items:center;gap:0.75rem"><ui-svg data-src="/statics/sites/localhost/images/symbol.svg" class="svg-sm svg-color-accent"></ui-svg><ui-svg data-src="/statics/sites/localhost/images/text.svg" class="svg-lg svg-color"></ui-svg></div>`
        },
        {
            name: "Brand text — glitched_v0",
            code: `<h1 class="glitch-text"><span aria-hidden="true">AKAO.IO</span>AKAO.IO<span aria-hidden="true">akao.io</span></h1>`,
            setup: (preview) => {
                const el = preview.querySelector(".glitch-text")
                let timer
                const fire = () => {
                    el.classList.add("is-glitching")
                    timer = setTimeout(() => {
                        el.classList.remove("is-glitching")
                        timer = setTimeout(fire, 22000 + Math.random() * 12000)
                    }, 1800)
                }
                fire()
                return () => clearTimeout(timer)
            }
        }
    ]
}
