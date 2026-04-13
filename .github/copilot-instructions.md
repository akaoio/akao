# akao - AI Coding Agent Instructions

## Project Overview
Framework-free, serverless, local-first eCommerce engine built with pure Web Components. All UI is pre-rendered static HTML per locale (currently 18 locales). No React, Vue, or framework dependencies—just native browser APIs. Includes optional DeFi/blockchain features (EVM chains, Uniswap V2/V3 DEX, on-chain wallets).

The current mindset is broader than "no framework": trust the web platform before escaping it, keep deployment static and simple, prefer explicit verifiability over trust theater, and push capability toward the browser and the user whenever possible. See `docs/phylosophy/README.md` for the higher-level worldview.

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
        // Clean up subscriptions — ALWAYS required
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

// Load with hash validation (auto-cached in IndexedDB)
const data = await DB.get(["statics", "items", "organic-green-tea", "meta.json"])

// DB.path() converts numeric IDs to directory structure
DB.path(12345) // => ["12", "34", "5"]
// Used for geo data: geo/12/34/5.json
```

Hash files (`.hash`) contain SHA-256 checksums. If hash matches cached hash, serve from IndexedDB—no network request.

### 5. Build System (Three Separate Builds)
**Run `npm run build:core` first** after any `src/` changes.

```bash
npm start              # build:core + dev server (recommended for development)
npm run build:core     # YAML→JSON, routes, i18n, hashes, forex rates
npm run build:crypto   # blockchain ABIs, chain configs, DEX pool lists
npm run build:geo      # GeoNames data (~12M records, ~5 min) 
npm run fix:geo        # fix specific geo entries without full rebuild
```

Build outputs to `build/` (gitignored). Creates structure:
```
build/
├── en/index.html, item/[slug]/index.html
├── fr/index.html, ...  (18 locales)
├── core/               — copied JS modules
├── UI/                 — copied components
├── statics/
│   ├── locales/        — per-locale i18n JSON
│   ├── items/          — product data + pagination
│   ├── chains/         — blockchain configs
│   ├── ABIs/           — contract ABIs
│   └── hashes/         — SHA-256 integrity hashes
└── geo/12/34/5.json    — hierarchical GeoNames data
```

**DO NOT** edit files in `build/`—they're regenerated. Edit `src/` only.

### 6. Routing (File-System Based)
Routes are defined by file structure in `src/UI/routes/`. Bracket notation for dynamic segments:

```
src/UI/routes/
├── home/index.js              → /
├── item/[item]/index.js       → /item/[item]
├── checkout/index.js          → /checkout
├── deposit/index.js           → /deposit
├── withdraw/index.js          → /withdraw
├── order/index.js             → /order
├── showcase/index.js          → /showcase
├── swap/index.js              → /swap
├── pools/index.js             → /pools
├── game/[game]/index.js       → /game/[game]
├── item/[game]/[item]/index.js→ /item/[game]/[item]
├── tag/[tag]/index.js         → /tag/[tag]
├── dispute/index.js           → /dispute
├── profile/index.js           → /profile
└── test/index.js              → /test
```

Router in [src/core/Router.js](src/core/Router.js) extracts locale, path params, and search params. Always locale-prefixed: `/en/item/some-slug`.

```javascript
// Router.process() parses URLs including search params
Router.process({ path: "/fr/item/organic-tea?chain=ETH&currency=USDT" })
// => { locale: {code: "fr"}, params: { item: "organic-tea", chain: "ETH", currency: "USDT" }, route: "/item/[item]" }
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
```

**Array paths**: `["a", "b", "c.json"]` → joins with `/` separator.

### 9. Component Communication
- **Props**: Via HTML attributes or constructor options (e.g., `SELECT` component)
- **Events**: `this.dispatchEvent(new CustomEvent("change", { detail: value }))`
- **Context**: Global state for theme, locale, fiat currency, route, user, auth
- **States.on()**: Subscribe to specific state keys

### 10. GeoNames Hierarchical Data
Geo data uses numeric IDs split into path segments via `DB.path()`. Each location has `parent` and `children` arrays.

```javascript
// Load geo location 12345 → geo/12/34/5.json
const id = DB.path(12345) // ["12", "34", "5"]
const geo = await DB.get(["geo", ...id.with(-1, `${id.at(-1)}.json`)])
// geo = { id: 12345, name: "Paris", parent: 123, children: [12346, 12347] }
```

Traverse hierarchy by following `parent`/`children`. See [src/UI/components/geo/index.js](src/UI/components/geo/index.js) for cascade select implementation.

### 11. Authentication (WebAuthn + SEA)
Authentication state lives in `Access` (a `States` instance in [src/core/Access.js](src/core/Access.js)):

```javascript
import { Access } from "/core/Access.js"
// { authenticated, id, pub, pair, wallet, avatar }

Access.on("authenticated", ({ value }) => { ... })
```

Key pair is deterministically derived from WebAuthn credential hash—no seed phrases, no passwords. Wallet seed = `sha256(pair.priv + walletId)`. Multiple wallets per user supported.

### 12. Blockchain / DeFi Layer
Optional on-chain features (skip if not working on DeFi routes):

- **Chain** ([src/core/Chain.js](src/core/Chain.js)): EVM chain connection (HTTPS + WebSocket RPC). Registered in `Stores.Chains`.
- **Dex** ([src/core/Dex.js](src/core/Dex.js)): Uniswap V2/V3 DEX instance. Registered in `Stores.Dexs`.
- **Wallet** ([src/core/Wallet.js](src/core/Wallet.js)): Derives key pair from `Access.seed`, wraps ethers.js signer.
- **Construct** ([src/core/Construct.js](src/core/Construct.js)): App bootstrap helpers (`Construct.Site()`, `Construct.Chains()`, `Construct.Dexs()`, `Construct.Wallet()`).
- **Stores** ([src/core/Stores.js](src/core/Stores.js)): Global singletons—`Indexes`, `Statics`, `Lives`, `Chains`, `Dexs`, `Wallets`, `Elements`.

### 13. Threading
The app uses a thread manager ([src/core/Threads.js](src/core/Threads.js)) to run background work:
- **main** thread: `src/core/threads/main.js` (app bootstrap / main context)
- **update** worker: `src/core/threads/update.js` (background data sync, Web Worker)

Bootstrapped via [src/core/Launcher.js](src/core/Launcher.js).

### 14. Shared Stores
[src/core/Stores.js](src/core/Stores.js) exports global singletons used across the codebase:

```javascript
import { Indexes, Statics, Lives, Chains, Dexs, Wallets, Elements } from "/core/Stores.js"

// Indexes: IndexedDB stores (Hashes, Statics, Auth, Lives, Cart)
// Statics: runtime-populated site config, locales, fiats, chains, etc.
// Chains / Dexs / Wallets: registered blockchain instances
```

## Development Workflows

### Adding a New Component
1. Create directory: `src/UI/components/my-component/`
2. Add `index.js`, `template.js`, `styles.css.js`
3. Follow the component pattern (see [src/UI/components/select/index.js](src/UI/components/select/index.js))
4. Import in parent or route: `import "/UI/components/my-component/index.js"`
5. Rebuild: `npm run build:core`

### Adding a New Route
1. Create `src/UI/routes/my-route/index.js` and `template.js`
2. Export class extending `HTMLElement` (name in UPPERCASE)
3. Rebuild—route auto-registered by build system
4. Access at `/{locale}/my-route` (e.g., `/en/my-route`)

### Adding Translation Keys
1. Create `src/statics/i18n/my-key.yaml` with all 18 locale codes
2. Run `npm run build:core` to generate JSON
3. Access: `Context.get(["dictionary", "myKey"])`

### Modifying Geo Data
**WARNING**: Geo rebuild takes ~5 minutes and processes 12M+ records. Use `npm run fix:geo` for specific fixes. Source data in `geo/allCountries.txt` (GeoNames dump).

## Common Pitfalls

1. **Don't use JSX syntax**—this isn't React. Use tagged template literals `` html`...` ``
2. **Don't forget Shadow DOM**—styles and queries are scoped: `this.shadowRoot.querySelector()`
3. **Clean up subscriptions**—always unsubscribe in `disconnectedCallback()`
4. **Rebuild after changes**—`src/` changes require `npm run build:core`
5. **Import paths start with `/`**—absolute from build root: `import X from "/core/UI.js"` not `../core/UI.js`
6. **Don't mutate Context directly**—use `Context.set({ key: value })` not `Context.states.key = value`
7. **Use DB.get() not fetch()**—for static files to leverage hash caching
8. **Never edit `build/`**—all files there are auto-generated

## Key Files Reference
- [src/core/UI.js](src/core/UI.js) → Template engine exports (`html`, `render`, `css`)
- [src/core/States.js](src/core/States.js) → Proxy-based reactive state
- [src/core/Context.js](src/core/Context.js) → Global state singleton (theme, fiat, locale, route…)
- [src/core/Router.js](src/core/Router.js) → URL pattern matching + search params
- [src/core/DB.js](src/core/DB.js) → Hash-validated static file loader
- [src/core/FS.js](src/core/FS.js) → Universal file operations (Node + browser)
- [src/core/Access.js](src/core/Access.js) → WebAuthn auth state
- [src/core/Stores.js](src/core/Stores.js) → Shared global singletons
- [src/core/Construct.js](src/core/Construct.js) → App initialization helpers
- [src/core/Chain.js](src/core/Chain.js) → EVM blockchain chain instance
- [src/core/Dex.js](src/core/Dex.js) → DEX instance (Uniswap V2/V3)
- [src/core/Wallet.js](src/core/Wallet.js) → Crypto wallet key derivation
- [src/core/Threads.js](src/core/Threads.js) → Thread manager
- [src/core/Launcher.js](src/core/Launcher.js) → Thread bootstrap entry point
- [src/core/Forex.js](src/core/Forex.js) → Fiat exchange rate management
- [src/core/Cart.js](src/core/Cart.js) → Shopping cart logic with IndexedDB persistence
- [builder/core.js](builder/core.js) → Core build system
- [dev.js](dev.js) → Dev server with hot reload

## Testing
Run tests against the real `build/` runtime: `npm test` (same as `npm run test:core`) for the default build-first core suite, `npm run test:build` for build artifact validation only, `npm run test:browser` for the Node headless runtime runner, `npm run test:playwright` for browser assertions against the live app runtime, `npm run test:isomorphic` for the combined Node-headless + browser-runtime flow, and `npm run test:geo` for geo data integrity.

Testing philosophy matters here: prefer real boot paths over narrow module verification, prefer generated `build/` artifacts over source-only assumptions, and prefer real dependencies over mocks whenever practical.

## Performance Philosophy
- **No Virtual DOM**: Direct DOM manipulation, no diffing overhead
- **Hash-based caching**: Files only re-fetched if content changed
- **Pre-rendered HTML**: Perfect Lighthouse scores, instant FCP
- **Primitive embedding**: Simple values embedded in HTML string (avoids marker overhead)
- **Static imports**: ES modules, tree-shakeable, HTTP/2 friendly
- **Web Workers**: Background threads for data sync without blocking UI

## Swarm Protocol

Đây là dự án làm việc nhóm giữa người và AI agents. Mọi agent PHẢI tuân thủ giao thức sau.

### Bắt đầu mỗi session
```bash
node swarm.js history       # đọc ngữ cảnh từ swarm
```
Sau đó đọc `swarm/notes.md` để nắm lại tư duy từ session trước.

### Khi làm việc
```bash
node swarm.js send "start | task: <mô tả> | branch: <branch>"
node swarm.js send "done | task: <mô tả> | branch: <branch>"
node swarm.js send "blocked | <vấn đề> | cần: <yêu cầu>"
node swarm.js notes "<insight quan trọng cần nhớ>"
node swarm.js chats            # lấy group chat IDs (dùng khi setup lần đầu)
```

`send` tự prepend identity từ `swarm.yaml` và @mention tất cả bots còn lại — agent không cần làm thêm gì.

### Cấu hình
Swarm dùng 2 files:
- `swarm.shared.yaml` — commit lên git, chứa groups và danh sách agents (PUBLIC)
- `swarm.yaml` — git-ignored, chứa token + machine name (SECRET)

Nếu chưa có `swarm.yaml`, copy từ `swarm.example.yaml` và điền token. Config shared tự động merge vào khi load.

Xem [SWARM.md](../SWARM.md) để biết thêm chi tiết.
