# Shop - AI Coding Agent Instructions

## Project Overview
Framework-less, serverless eCommerce engine built with pure Web Components. All UI is pre-rendered static HTML per locale (19 languages). No React, Vue, or framework dependencies—just native browser APIs.

## Critical Architecture Patterns

### 1. Custom Template Engine (Zero Virtual DOM)
Uses tagged template literals, NOT JSX. The `html()` function from `/core/UI.js` creates template objects, NOT DOM nodes directly.

```javascript
import { html, render } from "/core/UI.js"

// Correct: html() returns template object, render() creates DOM
const template = html`<div>${value}</div>`
render(template, container)

// Array mapping with nested templates
html`<ul>${items.map(i => html`<li>${i}</li>`)}</ul>`
```

**Key difference from React**: Primitives are embedded directly in HTML string (optimization), complex values use markers. No diffing algorithm. See [src/core/UI/html.js](src/core/UI/html.js) and [src/core/UI/render.js](src/core/UI/render.js).

### 2. Web Components Structure
All components/routes are custom elements with Shadow DOM. Standard pattern:

```javascript
export class COMPONENT extends HTMLElement {
    constructor() {
        super()
        this.states = new States({ /* local state */ })
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
    }
    
    connectedCallback() {
        // Subscribe to Context or fetch data
        this.subscriptions.push(Context.on("key", this.render))
    }
    
    disconnectedCallback() {
        // Clean up subscriptions
        this.subscriptions.forEach(off => off())
    }
}

customElements.define("ui-component", COMPONENT)
```

**Always** clean up subscriptions in `disconnectedCallback()` to prevent memory leaks.

### 3. State Management (Proxy-Based Reactive)
Two state systems:
- **Context** (global): Singleton `States` instance in [src/core/Context.js](src/core/Context.js)
- **States** (local): Component-level reactive state in [src/core/States.js](src/core/States.js)

```javascript
// Global state
import { Context } from "/core/Context.js"
Context.on("theme", ({ value }) => console.log(value))
Context.set({ theme: "dark" })

// Component state
this.states = new States({ count: 0 })
this.states.on("count", this.render)
this.states.set({ count: 1 })
```

Uses ES6 Proxy to intercept assignments. Only notifies if value actually changed (deep equality via `same()` helper).

### 4. Static File Loading (DB Pattern)
`DB.get()` is NOT a database—it's a static file loader with hash-based caching using IndexedDB.

```javascript
import DB from "/core/DB.js"

// Load with hash validation (auto-cached)
const data = await DB.get(["statics", "items", "123", "meta.json"])

// DB.path() converts numeric IDs to directory structure
DB.path(12345) // => ["12", "34", "5"]
// Used for geo data: geo/12/34/5.json
```

Hash files (`.hash`) contain SHA-256 checksums. If hash matches IndexedDB cache, skip network request.

### 5. Build System (Two Separate Builds)
**Run `npm run build:core` first** after any `src/` changes. Build system is in [builder/core.js](builder/core.js).

```bash
npm run build:core   # YAML→JSON, generate routes, i18n, hashes
npm run build:geo    # Process GeoNames data (slow, ~5min)
npm start            # Auto-rebuilds core on file changes
```

Build outputs to `build/` (gitignored). Creates structure:
```
build/
├── en/index.html, item/[slug]/index.html, tag/[slug]/index.html
├── fr/index.html, item/[slug]/index.html, ...
├── statics/locales/en.json (aggregated i18n)
└── geo/12/34/5.json (hierarchical geo data)
```

**DO NOT** edit files in `build/`—they're regenerated. Edit `src/` only.

### 6. Routing (File-System Based)
Routes are defined by file structure in `src/UI/routes/`. Bracket notation for dynamic segments:

```
src/UI/routes/
├── home/index.js              → /
├── item/[item]/index.js       → /item/[item]
└── tag/[tag]/index.js         → /tag/[tag]
```

Router in [src/core/Router.js](src/core/Router.js) extracts locale and params. Always locale-prefixed: `/en/item/some-slug`.

```javascript
// Router.process() parses URLs
Router.process({ path: "/fr/item/organic-tea" })
// => { locale: {code: "fr"}, params: { item: "organic-tea" }, route: "/item/[item]" }
```

### 7. Internationalization (Build-Time Pre-rendering)
Translations in `src/statics/i18n/*.yaml` (one file per key). Build aggregates into `build/statics/locales/{code}.json`.

```yaml
# src/statics/i18n/cart.yaml
en: Cart
fr: Panier
es: Carrito
```

Access via Context: `Context.get(["dictionary", "cart"])`. Each locale gets separate static HTML files for SEO.

### 8. File System Abstraction (FS Module)
Universal file operations for Node.js and browser. In [src/core/FS.js](src/core/FS.js):

```javascript
import { load, write, copy } from "/core/FS.js"

// Browser: fetch(), Node: fs.readFileSync()
const data = await load(["path", "to", "file.json"])

// Auto-parses JSON/YAML/CSV based on extension
const config = await load(["config.yaml"]) // Returns parsed object

// Nested object loading
const multi = await load({
    items: ["statics", "items.json"],
    locales: ["statics", "locales.json"]
})
```

**Array paths**: `["a", "b", "c.json"]` → joins with correct separator (Node: `/`, Browser: `/` in URLs).

### 9. Component Communication
- **Props**: Via HTML attributes or constructor options (e.g., `SELECT` component)
- **Events**: `this.dispatchEvent(new CustomEvent("change", { detail: value }))`
- **Context**: Global state for theme, locale, currency, user
- **States.on()**: Subscribe to specific state keys

Example from [src/UI/components/geo/index.js](src/UI/components/geo/index.js):
```javascript
// Parent creates child with callback
const select = new SELECT({
    options: [...],
    change: event => this.states.set({ id: event.target.value })
})
```

### 10. GeoNames Hierarchical Data
Geo data uses numeric IDs split into path segments via `DB.path()`. Each location has `parent` and `children` arrays.

```javascript
// Load geo location 12345 → geo/12/34/5.json
const id = DB.path(12345) // ["12", "34", "5"]
const geo = await DB.get(["geo", ...id.with(-1, `${id.at(-1)}.json`)])
// geo = { id: 12345, name: "Paris", parent: 123, children: [12346, 12347] }
```

Traverse hierarchy by following `parent`/`children`. See [src/UI/components/geo/index.js](src/UI/components/geo/index.js) for cascade select implementation.

## Development Workflows

### Adding a New Component
1. Create directory: `src/UI/components/my-component/`
2. Add `index.js` (exports class), `template.js` (html template), `styles.js` (css)
3. Use pattern from existing components (e.g., [src/UI/components/select/index.js](src/UI/components/select/index.js))
4. Import in parent or route: `import "/UI/components/my-component/index.js"`
5. Rebuild: `npm run build:core`

### Adding a New Route
1. Create `src/UI/routes/my-route/index.js` and `template.js`
2. Export class extending `HTMLElement` (name in UPPERCASE)
3. Rebuild—route auto-registered by build system
4. Access at `/{locale}/my-route` (e.g., `/en/my-route`)

### Adding Translation Keys
1. Create `src/statics/i18n/my-key.yaml` with all locale codes
2. Rebuild to generate JSON
3. Access: `Context.get(["dictionary", "my-key"])`

### Modifying Geo Data
**WARNING**: Geo rebuild takes ~5 minutes and processes 12M+ records. Use `npm run fix:geo` for specific fixes. Source data in `geo/allCountries.txt` (GeoNames dump).

## Common Pitfalls

1. **Don't use JSX syntax**—this isn't React. Use tagged template literals `html\`...\``
2. **Don't forget Shadow DOM**—styles and queries are scoped: `this.shadowRoot.querySelector()`
3. **Clean up subscriptions**—always unsubscribe in `disconnectedCallback()`
4. **Rebuild after changes**—`src/` changes require `npm run build:core`
5. **Import paths start with `/`**—absolute from build root: `import X from "/core/UI.js"` not `../core/UI.js`
6. **Don't mutate Context directly**—use `Context.set({ key: value })` not `Context.states.key = value`
7. **Use DB.get() not fetch()**—for static files to leverage hash caching

## Key Files Reference
- [src/core/UI.js](src/core/UI.js) → Template engine exports
- [src/core/States.js](src/core/States.js) → Reactive state system
- [src/core/Context.js](src/core/Context.js) → Global state singleton
- [src/core/Router.js](src/core/Router.js) → URL pattern matching
- [src/core/DB.js](src/core/DB.js) → Static file loader with caching
- [src/core/FS.js](src/core/FS.js) → Universal file operations
- [builder/core.js](builder/core.js) → Build system implementation
- [dev.js](dev.js) → Dev server with hot reload

## Testing
Run tests: `npm test`. Core tests in [test.js](test.js). Currently tests core modules and geo data integrity.

## Performance Philosophy
- **No Virtual DOM**: Direct DOM manipulation, no diffing overhead
- **Hash-based caching**: Files only re-fetched if content changed
- **Pre-rendered HTML**: Perfect Lighthouse scores, instant FCP
- **Primitive embedding**: Simple values embedded in HTML string (avoids marker overhead)
- **Static imports**: ES modules, tree-shakeable, HTTP/2 friendly
