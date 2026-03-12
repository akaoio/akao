/**
 * UI template engine tests — browser only (requires DOM).
 */
import Test from "../Test.js"
import { html, render } from "../UI.js"

Test.describe("UI — html()", () => {

    Test.it("returns an object, not a DOM node", () => {
        const tmpl = html`<div>hello</div>`
        Test.assert.falsy(tmpl instanceof Node, "html() should not return a DOM node")
        Test.assert.equal(typeof tmpl, "object")
    })

    Test.it("has _isTemplateResult marker", () => {
        const tmpl = html`<p>test</p>`
        Test.assert.truthy(tmpl._isTemplateResult)
    })

    Test.it("embeds primitive string directly", () => {
        const tmpl = html`<span>${"hello"}</span>`
        // Primitive is embedded in HTML string — no marker values needed
        Test.assert.truthy(tmpl.html.includes("hello"))
    })

    Test.it("embeds primitive number directly", () => {
        const tmpl = html`<span>${42}</span>`
        Test.assert.truthy(tmpl.html.includes("42"))
    })

    Test.it("uses marker for nested TemplateResult", () => {
        const inner = html`<b>inner</b>`
        const outer = html`<div>${inner}</div>`
        // Complex value → stored in markerValues, not embedded in html string
        Test.assert.truthy(outer.values.length > 0)
    })

    Test.it("uses marker for arrays", () => {
        const items = [1, 2, 3]
        const tmpl = html`<ul>${items}</ul>`
        Test.assert.truthy(tmpl.values.length > 0)
    })

    Test.it("same static strings array reuses cached template on second call", () => {
        function make() { return html`<p>static</p>` }
        const a = make()
        const b = make()
        // When there are no dynamic values, the same TemplateResult is returned
        Test.assert.equal(a, b)
    })

    Test.it("different instances are not the same object", () => {
        const a = html`<p>${"a"}</p>`
        const b = html`<p>${"b"}</p>`
        Test.assert.notEqual(a, b)
    })

}, { browser: true })

Test.describe("UI — render()", () => {

    Test.it("mounts template HTML into container", () => {
        const container = document.createElement("div")
        render(html`<span>hello</span>`, container)
        Test.assert.truthy(container.querySelector("span"))
        Test.assert.equal(container.querySelector("span").textContent, "hello")
    })

    Test.it("renders nested templates", () => {
        const inner = html`<b>bold</b>`
        const outer = html`<div>${inner}</div>`
        const container = document.createElement("div")
        render(outer, container)
        Test.assert.truthy(container.querySelector("b"))
        Test.assert.equal(container.querySelector("b").textContent, "bold")
    })

    Test.it("renders array of templates", () => {
        const items = [1, 2, 3].map(i => html`<li>${i}</li>`)
        const container = document.createElement("div")
        render(html`<ul>${items}</ul>`, container)
        const lis = container.querySelectorAll("li")
        Test.assert.equal(lis.length, 3)
        Test.assert.equal(lis[0].textContent, "1")
        Test.assert.equal(lis[2].textContent, "3")
    })

    Test.it("renders into ShadowRoot", () => {
        const host = document.createElement("div")
        document.body.appendChild(host)
        const shadow = host.attachShadow({ mode: "open" })
        render(html`<p>shadow</p>`, shadow)
        Test.assert.truthy(shadow.querySelector("p"))
        Test.assert.equal(shadow.querySelector("p").textContent, "shadow")
        document.body.removeChild(host)
    })

    Test.it("calls attribute function with context", () => {
        const container = document.createElement("div")
        let ctx = null
        // Attribute functions must be standalone interpolations (e.g. ${fn}),
        // not values assigned to a named attribute (onclick=${fn}).
        // The engine calls fn({ element }) at render time.
        render(html`<button ${ ({ element }) => { ctx = element } }>click</button>`, container)
        Test.assert.truthy(ctx instanceof HTMLElement)
    })

}, { browser: true })
