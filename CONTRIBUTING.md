# Contributing to Shop

Thank you for considering contributing to Shop! It's people like you that make this project a great tool for the community.

## Code of Conduct

This project is committed to providing a welcoming and inspiring community for all. By participating, you are expected to maintain this standard. Please report unacceptable behavior to the project maintainers.

## How Can I Contribute?

### Reporting Bugs

Before filing a bug report, check existing issues to avoid duplicates. Include:

- **Clear, descriptive title**
- **Steps to reproduce** (exact commands, URLs, or code)
- **Expected vs actual behavior**
- **Browser / OS / Node.js version**
- **Console errors and screenshots**

### Suggesting Enhancements

- **Clear title**
- **Detailed description** of the feature and its benefits
- **Alternatives considered**

### Pull Requests

1. **Fork** the repo and create your branch from `main`
2. **Follow the coding style** (see below)
3. **Test your changes** in multiple browsers
4. **Update docs** if needed
5. **Write clear commit messages**
6. **Submit a pull request**

## Development Setup

### Prerequisites

- Node.js 18+ (build tools only)
- A modern browser (Chrome, Firefox, Safari, Edge)
- Git
- VS Code (recommended)

### VS Code Extensions

- `es6-string-html` (ID: `Tobermory.es6-string-html`) is required for HTML syntax highlighting inside JavaScript template literals.
- The repository includes `.vscode/extensions.json` — VS Code will auto-recommend this extension when you open the project.

### Setup Steps

```bash
# Clone your fork
git clone https://github.com/YOUR-USERNAME/shop.git
cd shop

# Add upstream remote
git remote add upstream https://github.com/akaoio/shop.git

# Install dependencies
npm install

# Start dev server
npm start
```

The dev server runs at `http://localhost:8080` with hot reload enabled.

### Build Commands

```bash
npm start                    # build:core + dev server (recommended for development)
npm run build                # full build (crypto + core + geo)
npm run build:core           # YAML→JSON, routes, i18n, hashes, forex rates
npm run build:crypto         # blockchain ABIs, chain configs, DEX pool lists
npm run build:geo            # GeoNames data (~12M records, ~5 min)
npm run scan:crypto          # scan on-chain data for currencies/pools
npm run fix:geo              # fix specific GeoNames entries without full rebuild
npm run format               # Prettier formatting
npm run lint                 # ESLint
npm run test                 # full test suite
npm run test:core            # core module tests
npm run test:geo             # geo data integrity tests
```

## Project Structure

```
shop/
├── src/
│   ├── core/          # Core systems (UI, States, Router, Chain, Dex, Wallet…)
│   ├── UI/
│   │   ├── components/  # 29 Web Components
│   │   ├── routes/      # 10 route handlers
│   │   ├── layouts/     # Page layouts
│   │   └── css/         # Global styles
│   └── statics/       # Data files (YAML)
│       ├── i18n/        # Translation files (200+)
│       ├── items/       # Product data
│       ├── sites/       # Site configurations
│       ├── chains/      # Blockchain configs
│       └── logistics/   # Shipping data
├── builder/           # Build system modules
├── build/             # Generated output (gitignored — never edit)
├── build.js           # Build entry point
└── dev.js             # Dev server
```

## Coding Style

### General Principles

1. **Framework-less** — Use native Web APIs only, no framework dependencies
2. **Web Standards** — Leverage modern browser capabilities
3. **Readability** — Code should be self-documenting
4. **Performance** — Optimize for speed and minimal payload

### JavaScript Style

We use Prettier for formatting. Run `npm run format` before committing.

**Key conventions:**

```javascript
// Use const/let, never var
const items = []
let count = 0

// Arrow functions for short callbacks
items.map(item => item.name)

// Traditional functions for methods/exports
function processData() { ... }

// Async/await over raw promises
async function loadData() {
    const data = await fetch(url)
    return data.json()
}

// Destructuring
const { name, price } = product

// Template literals
const message = `Hello ${name}`

// Optional chaining
const value = obj?.nested?.property
```

### Web Components Pattern

Follow this standard structure for every component:

```javascript
// src/UI/components/my-component/index.js
import template from "./template.js"
import { render } from "/core/UI.js"
import States from "/core/States.js"

export class MyComponent extends HTMLElement {
    constructor() {
        super()
        this.states = new States({ key: "value" })
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
        this.handleEvent = this.handleEvent.bind(this)
    }

    static get observedAttributes() {
        return ["data-attribute"]
    }

    attributeChangedCallback(name, oldValue, newValue) {
        if (oldValue === newValue) return
        // handle change
    }

    connectedCallback() {
        this.subscriptions.push(
            this.states.on("key", this.render.bind(this))
        )
    }

    disconnectedCallback() {
        // ALWAYS clean up subscriptions to prevent memory leaks
        this.subscriptions.forEach(off => off())
    }

    handleEvent(event) { ... }

    render() { ... }
}

customElements.define("ui-my-component", MyComponent)
export default MyComponent
```

### Component File Structure

```
my-component/
├── index.js          # Component class + customElements.define()
├── template.js       # html`` template
└── styles.css.js     # css`` styles
```

**template.js:**

```javascript
import { html } from "/core/UI.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <div class="container">
        <!-- markup -->
    </div>
`

export default template
```

**styles.css.js:**

```javascript
import { css } from "/core/UI.js"

export const styles = css`
    :host {
        display: block;
    }
`

export default styles
```

### CSS Style

- Use CSS custom properties for theming (`var(--text-primary)`, `var(--bg-primary)` etc.)
- Scope styles with `:host` in Shadow DOM
- Follow BEM naming: `.component__element--modifier`
- Mobile-first responsive design

### File Naming

- **Component directories:** lowercase with hyphens (`my-component/`)
- **JavaScript files:** camelCase (`myHelper.js`)
- **Classes:** PascalCase (`MyComponent`)
- **Custom elements:** `ui-` prefix, lowercase hyphenated (`ui-my-component`)

## Adding Features

### Creating a New Component

1. Create `src/UI/components/my-component/`
2. Add `index.js`, `template.js`, `styles.css.js`
3. Follow the component pattern above
4. Import in parent component or route
5. Add to README if user-facing
6. Run `npm run build:core`

### Creating a New Route

Routes are file-system based. Create a directory under `src/UI/routes/`:

```
src/UI/routes/my-route/index.js   →  /{locale}/my-route
src/UI/routes/item/[item]/index.js →  /{locale}/item/[slug]
```

The build system auto-detects new route directories and generates the corresponding HTML files. The router auto-matches paths at runtime.

**Dynamic route example:**

```javascript
// src/UI/routes/item/[item]/index.js
import { Context } from "/core/Context.js"

export class ITEM extends HTMLElement {
    connectedCallback() {
        const { item } = Context.get("params")
        // load item by slug
    }
}

customElements.define("route-item", ITEM)
```

After adding a route, run `npm run build:core` to regenerate static HTML files.

### Adding Translations

1. Create `src/statics/i18n/my-key.yaml` with all 19 locale codes:

```yaml
en: My String
fr: Ma Chaîne
es: Mi Cadena
de: Meine Zeichenfolge
ja: 私の文字列
zh: 我的字符串
ko: 내 문자열
ru: Моя строка
ar: سلسلة بلدي
hi: मेरी स्ट्रिंग
pt: Minha String
it: La Mia Stringa
vi: Chuỗi Của Tôi
th: สตริงของฉัน
he: המחרוזת שלי
ur: میری سٹرنگ
zh-TW: 我的字符串
no: Min Streng
```

2. Run `npm run build:core` to aggregate into locale JSON files.
3. Use in components: `Context.get(["dictionary", "myKey"])`

### Adding Products

1. Create `src/statics/items/my-product/`
2. Add `meta.yaml`:

```yaml
price: 29.99
sale: 24.99
currency: USD
tags:
  - category
  - featured
sku: PROD-001
attributes:
  - name: Size
    options: [S, M, L, XL]
```

3. Add locale translation files (`en.yaml`, `fr.yaml`, etc.):

```yaml
name: Product Name
description: Product description
details: Detailed specifications
```

4. Run `npm run build:core` — product appears in listings and static pages are generated.

### Adding Blockchain Support

1. Add chain config to `src/statics/chains/YOUR-CHAIN/configs.yaml`
2. Add ABIs to `src/statics/ABIs/` (or run `npm run build:crypto` to fetch)
3. Register DEXs in `src/statics/dexs.yaml`
4. Add the chain code to the site config: `chains: [YOUR-CHAIN]`

## Critical Pitfalls

1. **Don't use JSX** — this is not React; use tagged template literals `` html`...` ``
2. **Always use Shadow DOM** — `this.shadowRoot.querySelector()`, not `document.querySelector()`
3. **Always clean up subscriptions** — `disconnectedCallback()` must call every `off()` function
4. **Rebuild after source changes** — `src/` edits require `npm run build:core` before the browser sees them
5. **Import paths start with `/`** — absolute from build root: `/core/UI.js`, not `../../core/UI.js`
6. **Never mutate Context directly** — use `Context.set({ key: value })`, not `Context.states.key = value`
7. **Use `DB.get()` for static files** — not raw `fetch()`, to leverage hash-based caching
8. **Don't edit `build/`** — all files there are auto-generated

## Testing

### Running Tests

```bash
npm test              # full test suite (Node.js)
npm run test:core     # core module unit tests only
npm run test:geo      # geo data integrity tests
```

For live feedback during development, open `http://localhost:8080/en/test` in the browser. The `/test` route runs the same suite in the browser and shows pass/fail per test with inline error messages. You can re-run individual suites or only the failed tests without refreshing the page.

### Test Runner (`src/core/Test.js`)

A minimal dual-environment test runner — no third-party dependencies. Works in Node.js (coloured console output, `process.exitCode = 1` on failure) and in the browser (returns structured results for the live UI).

**API:**

```javascript
import Test from "/core/Test.js"

Test.describe("Suite name", () => {

    Test.it("passes normally", () => {
        Test.assert.equal(result, expected)
        Test.assert.truthy(value)
        Test.assert.deepEqual(obj, { key: "val" })
        Test.assert.throws(() => badFn())
        await Test.assert.rejects(badPromise())
    })

    // Skipped in Node.js — requires DOM/localStorage
    Test.it("browser only", () => { ... }, { browser: true })

    // Never auto-run — requires hardware (WebAuthn, camera, etc.)
    Test.it("interactive", async () => { ... }, { interactive: true })

}, { browser: true }) // suite-level default
```

**Available assertions:** `equal`, `notEqual`, `deepEqual`, `truthy`, `falsy`, `throws`, `rejects`, `inRange`.

### Writing Tests

Test files live in `src/core/tests/` and are named `ModuleName.test.js`. Follow these conventions:

1. **Isolation** — save and restore any global/localStorage state you touch
2. **No side effects** — clean up DOM nodes, subscriptions, and IDB entries you create
3. **Browser flag** — add `{ browser: true }` to any test (or suite) that uses DOM APIs
4. **Interactive flag** — add `{ interactive: true }` only for tests that require real hardware
5. **One assert per test** — keep each `it()` focused on a single behaviour

After adding or modifying tests, rebuild and verify:

```bash
npm run build:core
npm run test:core
```

### Existing Test Coverage

| File | Modules covered | Tests |
|---|---|---|
| `Events.test.js` | Events | 8 |
| `States.test.js` | States | 19 |
| `Utils.test.js` | all Utils helpers | 51 |
| `Router.test.js` | Router | 21 |
| `Forex.test.js` | Forex | 9 |
| `IDB.test.js` | IDB | 8 |
| `DB.test.js` | DB | 11 |
| `Cart.test.js` | Cart | 14 |
| `UI.test.js` | html(), render() | 13 |
| `Context.test.js` | Context helpers | 8 |
| `Access.test.js` | Access | 8 |
| `WebAuthn.test.js` | WebAuthn | 4 |

### Manual Testing Checklist

- [ ] Test in multiple browsers (Chrome, Firefox, Safari, Edge)
- [ ] Test responsive design (mobile, tablet, desktop)
- [ ] Test with different locales (especially RTL: `ar`, `he`, `ur`)
- [ ] Check browser console for errors or warnings
- [ ] Verify Network tab shows correct cache behavior

### Checklist Before Submitting

- [ ] Code follows the style guide
- [ ] Tested in multiple browsers
- [ ] No console errors or warnings
- [ ] Translations added for all 19 locales (if new UI strings)
- [ ] Build completes without errors (`npm run build:core`)
- [ ] No merge conflicts with `main`

## Commit Messages

Format: `type: Description`

```
feat: add <ui-notifications> component
fix: correct search param parsing in Router
docs: update component creation guide
style: format code with Prettier
refactor: simplify Cart state update logic
perf: avoid re-render when state unchanged
test: add unit tests for DB hash validation
chore: update ethers to 6.16
```

**Types:** `feat`, `fix`, `docs`, `style`, `refactor`, `perf`, `test`, `chore`

## Questions?

- **Issues:** [GitHub Issues](https://github.com/akaoio/shop/issues)
- **Discussions:** [GitHub Discussions](https://github.com/akaoio/shop/discussions)

## Additional Resources

- [Web Components Guide](https://developer.mozilla.org/en-US/docs/Web/Web_Components)
- [Shadow DOM v1](https://developers.google.com/web/fundamentals/web-components/shadowdom)
- [Custom Elements v1](https://developers.google.com/web/fundamentals/web-components/customelements)
- [ES Modules](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Guide/Modules)
- [WebAuthn Guide](https://webauthn.guide/)
- [ethers.js Docs](https://docs.ethers.org/)

---

Thank you for contributing to Shop! 🎉
