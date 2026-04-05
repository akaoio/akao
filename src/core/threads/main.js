import { events } from "/core/Events.js"
import { Progress } from "/core/Progress.js"
import { Statics } from "/core/Stores.js"
import { merge } from "/core/Utils.js"
import { Construct } from "/core/Construct.js"
import Thread from "/core/Thread.js"
import { Context } from "/core/Context.js"
import Router from "/core/Router.js"
import DB from "/core/DB.js"
import styles from "/css/global.css.js"
import ACCESS from "/UI/components/access/index.js"
import { SPLASH } from "/UI/components/splash/index.js"
import { DEV } from "/core/Utils/environment.js"

const thread = new Thread()
const components = {}
let devtools = null
document.head.appendChild(styles)

events.on("authenticate", () => {
    globalThis.threads.call({
        thread: "onchain",
        method: "authenticate"
    })
})

Progress.set({
    Site: false,
    Chains: false,
    Dexs: false,
    Wallets: false,
    GDB: false,
    User: false,
    Context: false
})

let root = document.getElementById("root")
if (!root) {
    // Create root element for app ui
    root = document.createElement("div", { id: "root" })
    document.body.appendChild(root)
}

async function render(state = {}) {
    // Render new page
    state.path = state?.path || Context.get("path") || globalThis?.location?.pathname
    if (!state?.route || !state?.locale || !state.params) state = { ...state, ...Router.process({ path: state.path }) }
    state.route = state?.route || Context.get("route") || "home"
    if (state.route?.startsWith("showcase") && !DEV) state = { ...state, ...Router.process({ path: "/" }) } // make sure /showcase is only accessible in dev mode, otherwise fallback to home route
    state.locale = state?.locale || Context.get("locale") || Statics?.locales?.find?.((e) => e.code === Statics?.site?.locale) || Statics?.locales?.[0]
    state.params = state?.params || Context.get("params") || {}
    const component = components[state.route] || (await import(`/UI/routes/${state.route}/index.js`))
    if (!component) return
    components[state.route] = component
    // Name of the component class
    const name = state.route?.replace("-", "").toUpperCase()
    // Every component must have a class with the same name as the route in uppercase
    if (!component[name] && !component?.default) return
    const Component = component[name] || component?.default
    const element = new Component()
    root.replaceChildren(element)
    Router.setHistory(state)
    Router.setHead({ title: Statics?.dictionary?.[state.route?.replace?.("-", "")?.toLowerCase?.()] || "" })
    if (DEV && state.route === "home" && !devtools) {
        const { DEVTOOLS } = await import("/UI/components/devtools/index.js")
        devtools = new DEVTOOLS()
        document.body.appendChild(devtools)
    } else if (DEV && state.route !== "home" && devtools) {
        devtools.remove()
        devtools = null
    }
}

function splash(state = false) {
    let splash = document.querySelector("ui-splash")
    if (!splash) {
        splash = new SPLASH()
        document.body.prepend(splash)
    }
    splash.switch(state)
    return state
}

thread.init = async function () {
    const { site } = Statics
    if (!site) throw new Error("No site configs found during preload")
    splash(true)
    merge(Statics, {
        routes: await DB.get(["statics", "routes.json"]),
        locales: await DB.get(["statics", "locales.json"]),
        fiats: await DB.get(["statics", "fiats.json"]),
        themes: await DB.get(["statics", "themes.json"])
    })
    Progress.set({ Site: true })
    globalThis.threads.call({ thread: "torrent", method: "init" })
    Progress.set({ Chains: await Construct.Chains() })
    Progress.set({ Dexs: await Construct.Dexs() })
    Progress.set({ Wallets: await Construct.Wallets() })
    Progress.set({ GDB: await Construct.GDB() })
    // Listen to the popstate event, which is triggered when the user navigates back to the previous page
    // Updates Context with the new route info
    globalThis.addEventListener("popstate", () => {
        // Use stored history state (has correct params) rather than re-parsing the URL
        // Falls back to Router.process() only if history.state is unavailable
        const state = globalThis.history.state ? { ...globalThis.history.state } : Router.process()
        if (Context.get("locale")?.code !== state.locale?.code) Router.setLocale(state.locale.code)
        // Update params and route in Context before render so connectedCallback reads correct values
        Context.set({ params: state.params, route: state.route })
        render(state)
    })
    // Pass full Context state so render() preserves search params in history
    Context.on("path", ({ value: path }) => render({ path, route: Context.get("route"), locale: Context.get("locale"), params: Context.get("params") }))
    Context.on("locale", ({ value: locale }) => Router.setLocale(locale.code))
    Progress.set({ Context: await Construct.Context() })
    splash(false)
    const access = new ACCESS()
    document.body.appendChild(access)
}
