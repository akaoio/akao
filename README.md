# Shop 🛍️

A modern **serverless eCommerce engine** built with pure Web Components. Framework-free, fully static, and ready for the decentralized web.

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Web Components](https://img.shields.io/badge/Web-Components-29ABE2.svg)](https://www.webcomponents.org/)
[![No Framework](https://img.shields.io/badge/Framework-None-green.svg)](docs/thoughts/the-philosophy-of-framework-less.md)

## ✨ Features

- 🌍 **19 Languages** - Full internationalization with static routes per locale for optimal SEO
- 💱 **Multi-Currency** - Fiat currencies + on-chain crypto (ETH, BSC, and testnets)
- 🚀 **Serverless** - Pure static files deployable anywhere (Netlify, Vercel, GitHub Pages, S3)
- 🧩 **Web Components** - 29 native custom elements, no framework dependencies
- 🔐 **Passwordless Auth** - WebAuthn passkeys (Face ID, Touch ID, Windows Hello)
- 🔗 **DeFi Integration** - Uniswap V2/V3 DEX support, on-chain wallet, token swaps
- 📡 **Decentralized** - Optional GunDB integration for peer-to-peer data sync
- ⚡ **Fast** - Pre-rendered routes, hash-based caching, offline-first capabilities
- 🎨 **Themeable** - Built-in light/dark mode with CSS custom properties
- 📦 **Multi-tenant** - Different domains per site with shared infrastructure
- 🧵 **Multi-threaded** - Web Worker thread system for background tasks

## 🚀 Quick Start

### Prerequisites

- Node.js 18+ (for build tools only)
- No runtime dependencies
- VS Code (recommended) with `es6-string-html` for HTML syntax highlighting in JavaScript template literals

VS Code users: this repository ships with `.vscode/extensions.json`, so VS Code will prompt you to install the recommended extension automatically.
When prompted, click **Install All**. If the prompt does not appear, open Extensions and run the **@recommended** filter for this workspace.

### Installation

```bash
# Clone the repository
git clone https://github.com/akaoio/shop.git
cd shop

# Install dev dependencies
npm install

# Build and start dev server
npm start
```

The dev server will open at `http://localhost:8080` with hot reload enabled.

### Development Workflow

```bash
# Start dev server (auto runs build:core first)
npm start

# Full build (crypto + core + geo)
npm run build

# Individual build steps
npm run build:core       # YAML→JSON, routes, i18n, hashes, forex rates
npm run build:crypto     # Blockchain ABIs, chain configs, DEX pools
npm run build:geo        # GeoNames data (~5 min, 12M+ records)

# Crypto data scanning
npm run scan:crypto
npm run scan:crypto:currencies
npm run scan:crypto:pools

# Fix specific geo entries
npm run fix:geo

# Testing
npm test
npm run test:core
npm run test:geo

# Code quality
npm run format   # Prettier
npm run lint
npm run lint:fix
```

## 📁 Project Structure

```
shop/
├── src/                      # Source files (never edit build/)
│   ├── core/                 # Core systems
│   │   ├── UI/              # Template engine (html, render, css)
│   │   ├── States/          # Reactive state internals
│   │   ├── IDB/             # IndexedDB wrapper internals
│   │   ├── FS/              # File system abstraction internals
│   │   ├── Utils/           # Utilities (crypto, data, environment…)
│   │   ├── Chains/          # Blockchain architecture (EVM)
│   │   ├── DeFi/            # DEX integrations (Uniswap V2, V3)
│   │   ├── threads/         # Web Worker thread scripts
│   │   ├── Access.js        # WebAuthn authentication state
│   │   ├── Cart.js          # Shopping cart logic
│   │   ├── Chain.js         # Blockchain chain instance
│   │   ├── Construct.js     # App initialization helpers
│   │   ├── Context.js       # Global reactive state
│   │   ├── DB.js            # Hash-validated static file loader
│   │   ├── Dex.js           # DEX instance (V2/V3)
│   │   ├── Events.js        # Universal event bus (browser + Node.js)
│   │   ├── Forex.js         # Foreign exchange rate management
│   │   ├── GDB.js           # GunDB integration (Gun + SEA)
│   │   ├── IDB.js           # IndexedDB wrapper
│   │   ├── Launcher.js      # Thread bootstrap
│   │   ├── Progress.js      # Global progress state
│   │   ├── Router.js        # Pattern-based routing
│   │   ├── States.js        # Proxy-based reactive state
│   │   ├── Stores.js        # Shared stores (IDB indexes, chains, wallets…)
│   │   ├── Thread.js        # Single thread abstraction
│   │   ├── Threads.js       # Thread manager (main + worker threads)
│   │   ├── Wallet.js        # Crypto wallet (key derivation, balance, send)
│   │   └── WebAuthn.js      # WebAuthn/FIDO2 passkey wrapper
│   ├── UI/                   # User interface
│   │   ├── components/      # 29 reusable web components
│   │   ├── layouts/         # Page layouts
│   │   ├── routes/          # 10 route handlers
│   │   └── css/             # Global styles
│   └── statics/              # Static data and content
│       ├── i18n/            # 200+ translation files
│       ├── items/           # Product data (YAML)
│       ├── sites/           # Multi-tenant site configs
│       ├── chains/          # Blockchain configs (ETH, BSC…)
│       ├── ABIs/            # Smart contract ABIs
│       ├── logistics/       # Shipping carriers and routes
│       ├── locales.yaml     # Language definitions (19 locales)
│       ├── fiats.yaml       # Fiat currency definitions
│       ├── themes.yaml      # Theme configurations
│       ├── dexs.yaml        # DEX definitions
│       ├── forex.yaml       # Cached forex rates
│       └── system.yaml      # System-wide settings (pagination etc.)
├── build/                    # Generated output (gitignored)
├── builder/                  # Build system modules
│   ├── core.js              # Main core build runner
│   ├── crypto.js            # Crypto/blockchain build runner
│   ├── geo.js               # Geo data build runner
│   ├── core/                # Core build helpers (routes, i18n, hash…)
│   └── geo/                 # Geo build helpers
├── build.js                  # Build entry point (selects builder by arg)
├── scan.js                   # Crypto data scanner
├── dev.js                    # Dev server with hot reload
└── docs/                     # Documentation and thoughts
```

## 🏗️ Architecture

### Core Philosophy

This project embraces **framework-less development**, leveraging native web standards:

- **Web Components API** - Custom elements with Shadow DOM
- **ES Modules** - Native JavaScript modules
- **Template Literals** - Zero-overhead templating
- **Web Standards** - No proprietary abstractions

See [the-philosophy-of-framework-less.md](docs/thoughts/the-philosophy-of-framework-less.md) for detailed rationale.

### UI System

The custom template engine provides React/Vue-like DX without the overhead:

```javascript
import { html, render } from "/core/UI.js"

// Define template
const template = html`
    <div class="card">
        <h2>${title}</h2>
        <p>${description}</p>
        ${items.map(item => html`
            <span>${item}</span>
        `)}
    </div>
`

// Render to DOM
render(template, container)
```

**Key Features:**
- Nested templates and arrays
- Attribute event handlers
- Automatic primitive value embedding (performance optimization)
- No Virtual DOM, no diffing — direct DOM manipulation

### State Management

Reactive state with ES6 Proxy, two layers:

| Layer | Module | Scope |
|---|---|---|
| Global | `Context.js` | Shared across the entire app (theme, fiat, locale, route…) |
| Local | `States.js` | Component-level, garbage-collected with the component |

```javascript
import { Context } from "/core/Context.js"

// Subscribe to changes
Context.on("theme", ({ value }) => console.log("Theme:", value))

// Update state (triggers all subscribers)
Context.set({ theme: "dark" })
```

### Routing

Pattern-based router with dynamic segments and search param support:

```javascript
// Route defined by file structure
// src/UI/routes/item/[item]/index.js → /item/[item]

// URL parsing with search params
Router.process({ path: "/fr/item/organic-tea?sort=price&page=2" })
// => { locale: {code: "fr"}, params: { item: "organic-tea", sort: "price", page: "2" }, route: "/item/[item]" }
```

Routes are always locale-prefixed: `/en/`, `/fr/`, etc.

### Multi-threading

```javascript
// Launcher.js bootstraps two threads:
// - main thread  (threads/main.js)
// - update worker (threads/update.js) — background data sync
```

### Blockchain / DeFi

Optional on-chain features built on `ethers.js`:

- **Chain** — connects to EVM networks (HTTPS + WebSocket RPC)
- **Wallet** — derives key pairs from WebAuthn credential + wallet ID (no seed phrases)
- **Dex** — Uniswap V2/V3 pool queries and swaps
- **Forex** — fiat exchange rates with 24-hour file cache

Supported networks (configurable per site): `ETH`, `ETH-Sepolia`, `BSC`, `BSC-Testnet`.

## 🌐 Internationalization

### Build-Time i18n

Static routes generated for each locale:

```
build/
├── en/index.html, item/[slug]/index.html, ...
├── fr/, de/, zh/, ... (19 locales total)
```

**Benefits:**
- Perfect SEO (separate URL per language)
- No JavaScript required for initial render
- Instant First Contentful Paint

### Translation Files

Each UI string has its own YAML file (`src/statics/i18n/`):

```yaml
# src/statics/i18n/cart.yaml
en: Cart
fr: Panier
es: Carrito
de: Warenkorb
```

Build aggregates into per-locale JSON files served at runtime. Access via `Context.get(["dictionary", "cart"])`.

## 🛠️ Build System

Three independent build pipelines, all triggered via `build.js`:

| Command | What it does |
|---|---|
| `npm run build:core` | YAML→JSON, static routes, i18n, hashes, forex |
| `npm run build:crypto` | Fetches ABIs, chain configs, DEX pool lists |
| `npm run build:geo` | Processes GeoNames (~12M records, ~5 min) |

**Build output** (in `build/`):

```
build/
├── en/index.html, item/[slug]/index.html, tag/[slug]/index.html
├── fr/, de/, ... (19 locales)
├── core/          — copied JS modules
├── UI/            — copied components
├── statics/
│   ├── locales/   — per-locale i18n JSON
│   ├── items/     — product data + pagination
│   ├── chains/    — blockchain configs
│   ├── ABIs/      — contract ABIs
│   └── hashes/    — SHA-256 integrity hashes
├── geo/           — hierarchical GeoNames data
└── images/        — copied assets
```

**DO NOT** edit files in `build/` — they are regenerated on every build.

## 🧩 Components

### Available Components (29)

**Navigation & Layout:**
- `<ui-header>`, `<ui-footer>`, `<ui-navigator>` — page structure
- `<ui-a>` — router-aware anchor element

**UI Controls:**
- `<ui-button>`, `<ui-picker>`, `<ui-modal>` — interactive controls
- `<ui-icon>` — icon renderer (Bootstrap Icons)
- `<ui-svg>` — inline SVG wrapper
- `<ui-select>` — enhanced select control
- `<ui-context>` — live value from Context state
- `<ui-splash>` — loading/splash screen

**Localization:**
- `<ui-locales>` — language switcher
- `<ui-fiats>` — fiat currency switcher
- `<ui-fiat>` — single currency display
- `<ui-themes>` — theme toggle (light/dark)

**Authentication & Identity:**
- `<ui-user>` — user profile display
- `<ui-access>` — WebAuthn authentication modal
- `<ui-signout>` — sign-out button
- `<ui-avatars>` — avatar selector
- `<ui-identicon>` — deterministic avatar from public key

**eCommerce:**
- `<ui-item>` — product card
- `<ui-items>` — paginated product grid
- `<ui-cart>` — shopping cart
- `<ui-profile>` — user profile editor

**Blockchain / Wallet:**
- `<ui-wallets>` — wallet selector (chain + currency aware)

**Geo & Other:**
- `<ui-geo>` — hierarchical geographic selector (country → region → city)
- `<ui-addresses>` — address book manager
- `<ui-notifications>` — toast notification system

### Creating a Component

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
    }

    connectedCallback() {
        this.subscriptions.push(
            this.states.on("key", this.render.bind(this))
        )
    }

    disconnectedCallback() {
        this.subscriptions.forEach(off => off())
    }

    render() {}
}

customElements.define("ui-my-component", MyComponent)
```

## 📄 Routes

| Route | Path | Description |
|---|---|---|
| Home | `/` | Product listing homepage |
| Item | `/item/[item]` | Product detail page |
| Checkout | `/checkout` | Checkout flow |
| Order | `/order` | Order confirmation / status |
| Deposit | `/deposit` | Crypto wallet funding |
| Withdraw | `/withdraw` | Crypto withdrawal |
| Inventory | `/inventory` | Seller inventory management |
| Dispute | `/dispute` | Order dispute handling |
| Profile | `/profile` | User profile settings |
| Test | `/test` | Development testing page |

## 🔐 Authentication

WebAuthn-based passwordless authentication:

```javascript
import { Access } from "/core/Access.js"

if (Access.get("authenticated")) {
    const { pair, wallet, avatar } = Access.states.get()
}

Access.on("authenticated", ({ value }) => {
    console.log("Auth state:", value)
})
```

**Features:**
- Platform authenticators (Touch ID, Face ID, Windows Hello, security keys)
- SEA key pair derived deterministically from WebAuthn credential hash (no seed phrases)
- Encrypted public key storage in GunDB
- Multi-wallet support (wallet ID selects BIP-32 derivation path)
- Avatar support with deterministic identicons

## 🗄️ Data Storage

| Layer | Module | Purpose |
|---|---|---|
| IndexedDB | `IDB.js` | Client-side persistent cache |
| GunDB | `GDB.js` | Decentralized graph database (optional) |
| Static Files | `DB.js` | Hash-validated JSON with auto-caching |
| Blockchain | `Chain.js` / `Wallet.js` | On-chain state and transactions |

```javascript
import DB from "/core/DB.js"

// Automatically hash-validated and IndexedDB-cached
const item = await DB.get(["statics", "items", "organic-green-tea", "meta.json"])
```

## 🎨 Theming

CSS custom properties with automatic system-preference detection:

```css
:host {
    color: var(--text-primary);
    background: var(--bg-primary);
}
```

Themes defined in `src/statics/themes.yaml`. Selected theme is persisted in `localStorage` and applied as `data-theme` on `<html>`.

## 🌍 Geo Data

Hierarchical geographic data from GeoNames (~12M locations):

```javascript
import DB from "/core/DB.js"

// DB.path() splits a numeric ID into directory path segments
DB.path(12345) // => ["12", "34", "5"]

const geo = await DB.get(["geo", "12", "34", "5.json"])
// { id: 12345, name: "Paris", parent: 123, children: [...] }
```

Used by `<ui-geo>` for cascading country → region → city selection.

## 🧪 Testing

The project ships a micro test runner (`src/core/Test.js`) with zero external dependencies that works in both Node.js and the browser.

### Running Tests

```bash
npm test              # full test suite (Node.js, coloured console output)
npm run test:core     # core module unit tests only
npm run test:geo      # geo data integrity tests
```

For a live interactive view, open `/{locale}/test` in the dev server (e.g. `http://localhost:8080/en/test`). The browser UI shows pass/fail per suite in real time, lets you re-run individual suites or only failed tests, and displays inline error messages.

### Test Coverage (12 files, 36 suites, 174 tests)

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
| `WebAuthn.test.js` | WebAuthn passkey lifecycle | 4 |

Tests marked `{ browser: true }` are skipped in Node.js. Tests marked `{ interactive: true }` (e.g. WebAuthn hardware) are always skipped in automated runs and triggered manually in the browser UI.

## 🚢 Deployment

Deploy the `build/` directory to any static host:

```bash
npm run build

# Netlify
netlify deploy --dir=build --prod

# Vercel
vercel --prod
```

### Site Configuration

`src/statics/sites/your-site/configs.yaml`:

```yaml
name: mystore
brand:
  text: /statics/sites/mystore/images/branding.svg
  symbol: /statics/sites/mystore/images/logo.svg
favicon: /statics/sites/mystore/images/icon.svg
locale: en
fiat: USD
chains:
  - ETH
  - BSC
peers: []   # GunDB peers for decentralized sync
```

`src/statics/domains.yaml`:

```yaml
mystore.com: mystore
```

## 📚 Documentation

- [The Philosophy of Framework-less](docs/thoughts/the-philosophy-of-framework-less.md) — Why no framework?
- [WebAuthn PRF Extension](docs/WebAuthn-PRF-Extension.md) — Passkey deep-dive
- [White Paper Draft](docs/thoughts/white-paper-draft.md) — 4-party escrow design

## 🤝 Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for:

- Development setup
- Code style guide
- Component creation guide
- Commit message format

## 📄 License

MIT License — see [LICENSE](LICENSE) for details.

---

**Built with ❤️ using pure Web Components**
