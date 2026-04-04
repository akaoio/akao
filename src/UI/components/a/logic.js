import Router from "/core/Router.js"

export class Logic {
    static href(to, locale) {
        const router = Router.process({ path: to, locale })
        return router.path
    }

    static navigate(href) {
        Router.navigate(href)
    }
}

export default Logic
