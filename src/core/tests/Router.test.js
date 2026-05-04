import Test from "../Test.js"
import { Router } from "../Router.js"

// Minimal mock data — passed explicitly so Router never touches globals
const locales = [
    { code: "en", name: "English" },
    { code: "fr", name: "French" },
    { code: "zh-TW", name: "Traditional Chinese" },
    { code: "vi", name: "Vietnamese" }
]

const routes = [
    "/item/[item]",
    "/tag/[tag]",
    "/order",
    "/checkout",
    "/profile",
    "/deposit",
    "/withdraw",
    "/inventory",
    "/dispute",
    "/test"
]

const site = { locale: "en" }

const opts = { locales, routes, site }

Test.describe("Router — match()", () => {

    Test.it("matches static route with no params", () => {
        const params = Router.match(["checkout"], "/checkout")
        Test.assert.deepEqual(params, {})
    })

    Test.it("returns null on no match", () => {
        const params = Router.match(["unknown"], "/checkout")
        Test.assert.equal(params, null)
    })

    Test.it("extracts single dynamic segment [param]", () => {
        const params = Router.match(["item", "organic-tea"], "/item/[item]")
        Test.assert.deepEqual(params, { item: "organic-tea" })
    })

    Test.it("returns null when dynamic segment is missing", () => {
        const params = Router.match(["item"], "/item/[item]")
        Test.assert.equal(params, null)
    })

    Test.it("returns null when extra segments present on static route", () => {
        const params = Router.match(["checkout", "extra"], "/checkout")
        Test.assert.equal(params, null)
    })

    Test.it("matches catch-all [...param]", () => {
        const params = Router.match(["blog", "2024", "post"], "/blog/[...slug]")
        Test.assert.deepEqual(params, { slug: ["2024", "post"] })
    })

    Test.it("optional catch-all [[...param]] matches empty segments", () => {
        const params = Router.match([], "/[[...rest]]")
        Test.assert.truthy(params !== null)
    })

    Test.it("optional catch-all [[...param]] matches with segments", () => {
        const params = Router.match(["a", "b"], "/[[...rest]]")
        Test.assert.truthy(params !== null)
    })

})

Test.describe("Router — process()", () => {

    Test.it("extracts locale from path prefix", () => {
        const r = Router.process({ path: "/fr/checkout", ...opts })
        Test.assert.equal(r.locale.code, "fr")
    })

    Test.it("falls back to site locale when no locale in path", () => {
        const r = Router.process({ path: "/checkout", ...opts })
        Test.assert.equal(r.locale.code, "en")
    })

    Test.it("routes to home for bare locale path", () => {
        const r = Router.process({ path: "/en/", ...opts })
        Test.assert.equal(r.route, "home")
    })

    Test.it("routes to home for root path", () => {
        const r = Router.process({ path: "/", ...opts })
        Test.assert.equal(r.route, "home")
    })

    Test.it("extracts item dynamic param", () => {
        const r = Router.process({ path: "/en/item/organic-tea", ...opts })
        Test.assert.equal(r.route, "/item/[item]")
        Test.assert.equal(r.params.item, "organic-tea")
    })

    Test.it("extracts tag dynamic param", () => {
        const r = Router.process({ path: "/en/tag/electronics", ...opts })
        Test.assert.equal(r.route, "/tag/[tag]")
        Test.assert.equal(r.params.tag, "electronics")
    })

    Test.it("tag route respects locale prefix", () => {
        const r = Router.process({ path: "/fr/tag/soldes", ...opts })
        Test.assert.equal(r.locale.code, "fr")
        Test.assert.equal(r.route, "/tag/[tag]")
        Test.assert.equal(r.params.tag, "soldes")
    })

    Test.it("tag route merges search params", () => {
        const r = Router.process({ path: "/en/tag/electronics?currency=USDT", ...opts })
        Test.assert.equal(r.params.tag, "electronics")
        Test.assert.equal(r.params.currency, "USDT")
    })

    Test.it("tag path param overrides search param on conflict", () => {
        const r = Router.process({ path: "/en/tag/electronics?tag=other", ...opts })
        Test.assert.equal(r.params.tag, "electronics")
    })

    Test.it("path params override search params on conflict", () => {
        const r = Router.process({ path: "/en/item/tea-slug?item=other", ...opts })
        Test.assert.equal(r.params.item, "tea-slug")
    })

    Test.it("search params are merged into params", () => {
        const r = Router.process({ path: "/en/checkout?currency=USDT", ...opts })
        Test.assert.equal(r.params.currency, "USDT")
    })

    Test.it("multiple search params are all present", () => {
        const r = Router.process({ path: "/en/checkout?chain=ETH&currency=USDT", ...opts })
        Test.assert.equal(r.params.chain, "ETH")
        Test.assert.equal(r.params.currency, "USDT")
    })

    Test.it("unrecognised path with extension falls back to home", () => {
        const r = Router.process({ path: "/en/checkout.html", ...opts })
        // .html not stripped — no route match → falls back to home
        Test.assert.equal(r.route, "home")
    })

    Test.it("trailing slash is normalised in output path", () => {
        const r = Router.process({ path: "/en/checkout", ...opts })
        Test.assert.truthy(r.path.endsWith("/"))
    })

    Test.it("handles zh-TW locale code with hyphen", () => {
        const r = Router.process({ path: "/zh-TW/checkout", ...opts })
        Test.assert.equal(r.locale.code, "zh-TW")
    })

    Test.it("falls back to home route when path does not match any route", () => {
        const r = Router.process({ path: "/en/nonexistent/path", ...opts })
        Test.assert.equal(r.route, "home")
    })

    Test.it("explicit locale option overrides path locale", () => {
        const r = Router.process({ path: "/fr/checkout", locale: "en", ...opts })
        Test.assert.equal(r.locale.code, "en")
    })

})
