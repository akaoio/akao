import { BROWSER } from "./Utils/environment.js"
import States from "./States.js"
import { Statics } from "./Stores.js"

export const Context = new States({
    theme: getTheme(),
    fiat: getFiat(),
    referrer: null
})

globalThis.Context = Context

export function getTheme() {
    if (!BROWSER) return
    const memory = globalThis.localStorage ? globalThis.localStorage.getItem("theme") : null
    const system = globalThis.matchMedia?.("(prefers-color-scheme: dark)").matches ? "dark" : "light"
    const theme = memory || system || Statics.themes?.[0]?.code
    if (Statics.themes && theme && !Statics.themes.find((e) => e.code == theme)) return
    if (theme && theme !== memory && globalThis?.localStorage) globalThis.localStorage.setItem("theme", theme)
    if (globalThis?.document) globalThis.document.documentElement.dataset.theme = theme
    return theme
}

export function setTheme(theme) {
    if (globalThis.localStorage && globalThis.localStorage.getItem("theme") !== theme) globalThis.localStorage.setItem("theme", theme)
    if (globalThis.document) document.documentElement.dataset.theme = theme
    if (Context.get("theme") === theme) return
    Context.set({ theme })
}

export function getFiat() {
    const memory = globalThis.localStorage ? globalThis.localStorage.getItem("fiat") : null
    const code = memory || Statics.site?.fiat
    if (code && code !== memory && globalThis.localStorage) globalThis.localStorage.setItem("fiat", code)
    const fiat = Statics.fiats?.find?.((element) => element.code == code)
    if (!fiat) return Statics.fiats?.[0]
    return fiat
}

export function setFiat(code) {
    const fiat = Statics.fiats?.find?.((e) => e.code == code)
    if (!fiat) return
    if (globalThis.localStorage) globalThis.localStorage.setItem("fiat", code)
    // Don't update Context if it already reflects this fiat
    if (code === Context.get("fiat")?.code) return
    Context.set({ fiat })
}

export function getReferrer() {
    const memory = globalThis.localStorage ? globalThis.localStorage.getItem("referrer") : null
    if (!globalThis.location) return
    const code = new URLSearchParams(globalThis.location.search).get("r")
    const { gun } = globalThis
    return new Promise((resolve) => {
        if (memory) return resolve(memory)
        if (!code) return resolve()
        const scope = gun.get("#link").get(code)
        scope.on((referrer) => {
            // Only the first referrer is saved
            // This is to prevent the user from having more than 1 referrer
            if (!referrer) return
            scope.off()
            if (referrer !== memory && globalThis.localStorage) globalThis.localStorage.setItem("referrer", referrer)
            return resolve(referrer)
        })
    }).catch((error) => console.error(error))
}
