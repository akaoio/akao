# akao 🛍️

**akao** is a modern **serverless, local-first eCommerce engine** built with pure Web Components. It is framework-free, fully static, and designed for a more decentralized web.

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Web Components](https://img.shields.io/badge/Web-Components-29ABE2.svg)](https://www.webcomponents.org/)
[![No Framework](https://img.shields.io/badge/Framework-None-green.svg)](docs/thoughts/the-philosophy-of-framework-less.md)
[![Telegram](https://img.shields.io/badge/Telegram-akaoproject-2CA5E0.svg?logo=telegram)](https://t.me/akaoproject)

## ✨ Features

- 🌍 **18 Languages** - Full internationalization with static routes per locale for optimal SEO
- 💱 **Multi-Currency** - Fiat currencies + on-chain crypto (ETH, BSC, and testnets)
- 🚀 **Serverless** - Pure static files deployable anywhere (Netlify, Vercel, GitHub Pages, S3)
- 🧩 **Web Components** - Native custom elements with no framework dependency
- 🔐 **Passwordless Auth** - WebAuthn passkeys (Face ID, Touch ID, Windows Hello)
- 🔗 **DeFi Integration** - Uniswap V2/V3 DEX support, on-chain wallet, token swaps
- 📡 **Decentralized** - ZEN P2P graph sync for real-time data without a central server
- 📶 **WebRTC P2P** - Direct browser-to-browser DataChannel, signaling-agnostic (works with ZEN, HTTP, QR, anything)
- 🌊 **WebTorrent** - In-browser BitTorrent over WebRTC — seed and stream files without a server
- ⚡ **Fast** - Pre-rendered routes, hash-based caching, offline-first capabilities
- 🎨 **Themeable** - Built-in light/dark mode with CSS custom properties
- 📦 **Multi-tenant** - Different domains per site with shared infrastructure
- 🧵 **Multi-threaded** - Web Worker thread system for background tasks
- 💾 **SQLite in Browser** - Full SQL queries via SQLite WASM + OPFS, isolated in a dedicated worker

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
git clone https://github.com/akaoio/akao.git
cd akao

# Install dev dependencies
npm install

# Build and start dev server
npm start
```

The default dev surface is `http://localhost:8080`. If a hostname is mapped to the same site in `src/statics/domains.yaml` and resolves to this machine (for example `peer0.akao.io`), the same dev server can also be reached through that host. `npm start` restarts any matching `dev.js` / `market.js` listeners on ports `8080` and `8420` before starting a fresh stack.

In DEV, `Construct.Site()` patches `Statics.site.platform` with a deterministic fallback identity when `platform.pub` is missing. The fallback is derived from the shared seed `"seed"` via `zen.pair(null, { seed: "seed" })`, so local development and tests do not need a separately stored platform keypair.

### Development Workflow

```bash
# Restart dev stack, start ZEN relay, build crypto + core --dev, then serve app
npm start

# Start Node headless runtime through build/core/Launcher.js
npm start -- --headless
npm run start:headless

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
npm test              # same as test:core
npm run test:core     # build-first core suite + isomorphic runtime checks
npm run test:build    # verify generated build/ artifacts only
npm run test:browser  # build/core + Node headless browser-route runner
npm run test:playwright
npm run test:isomorphic
npm run test:geo      # separate geo integrity suite

# Code quality
npm run format   # Prettier
npm run lint
npm run lint:fix
```

### Production

`akao` is static at runtime, so production mode is just: build once, then serve `build/` over HTTPS.

```bash
# Build the production bundle
npm run build

# Start akao production server
npm run start:prod
```

Production mode binds **443** for the app and **80** for HTTP-to-HTTPS redirects. Unlike `zen`, it does **not** fall back to `~/cert.pem` or `~/key.pem`, so it will not collide with shared home-directory certificates on the server.

By default, `npm run start:prod` expects:

```text
.certs/prod-key.pem
.certs/prod-cert.pem
```

You can override those paths explicitly:

```bash
SSL_KEY=/absolute/path/to/key.pem SSL_CERT=/absolute/path/to/cert.pem npm run start:prod
```

If you want build + boot in one step:

```bash
npm run prod
```

To issue a Let's Encrypt certificate into akao's own `.certs/` folder:

```bash
npm run ssl -- --domain akao.io --email dev@akao.io --standalone
```

If you use standalone mode directly, run it with sudo/root because Let's Encrypt must reach port 80 during validation.

To install a systemd service for `prod.js`:

```bash
npm run service:install -- --service akao
```

To do the full production setup in one command:

```bash
npm run setup:prod -- --domain akao.io --email dev@akao.io
```

`setup:prod` now installs the service without starting it, launches a temporary ACME challenge server on port 80, issues the certificate in webroot mode, then starts the real production service.

## 📁 Project Structure

```
 akao/
├── src/                      # Source files (never edit build/)
│   ├── core/                 # Core systems
│   │   ├── UI/              # Template engine (html, render, css)
│   │   ├── States/          # Reactive state internals
│   │   ├── IDB/             # IndexedDB wrapper internals
│   │   ├── OPFS/            # Origin Private File System wrapper internals
│   │   ├── RTC/             # WebRTC DataChannel internals
│   │   ├── SQL/             # SQLite WASM client internals
│   │   ├── Torrent/         # WebTorrent client internals
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
│   │   ├── ZEN.js           # ZEN P2P graph + crypto runtime wrapper
│   │   ├── IDB.js           # IndexedDB wrapper
│   │   ├── RTC.js           # WebRTC DataChannel façade
│   │   ├── Launcher.js      # Thread bootstrap
│   │   ├── OPFS.js          # OPFS async file system wrapper
│   │   ├── Progress.js      # Global progress state
│   │   ├── Router.js        # Pattern-based routing
│   │   ├── SQL.js           # SQLite WASM client (worker-backed)
│   │   ├── States.js        # Proxy-based reactive state
│   │   ├── Stores.js        # Shared stores (IDB indexes, chains, wallets…)
│   │   ├── Thread.js        # Single thread abstraction
│   │   ├── Threads.js       # Thread manager (main + worker threads)
│   │   ├── Torrent.js       # WebTorrent façade (seed, add, stream)
│   │   ├── Wallet.js        # Crypto wallet (key derivation, balance, send)
│   │   └── WebAuthn.js      # WebAuthn/FIDO2 passkey wrapper
│   ├── UI/                   # User interface
│   │   ├── components/      # Reusable web components
│   │   ├── layouts/         # Page layouts
│   │   ├── routes/          # Route handlers
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

The project is grounded in a few durable convictions:

- **Native over ornamental abstraction** - trust the web platform before escaping it
- **Static and local-first by default** - push capability toward the browser and keep deployment simple
- **Verifiability over trust theater** - prefer explicit proofs, deterministic structure, and testable flows
- **User sovereignty over custody** - identity, data, and value should remain as close to the user as possible

For the broader worldview behind the codebase, start with [docs/phylosophy/README.md](docs/phylosophy/README.md).

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
// Launcher.js bootstraps three threads:
// - main thread  (threads/main.js)
// - update worker (threads/update.js) — background data sync
// - sql worker   (threads/sql.js)    — SQLite WASM + OPFS (dedicated worker required)
```

### SQLite / OPFS Storage

Full SQL capabilities in the browser via `@sqlite.org/sqlite-wasm` running inside a dedicated worker with OPFS as the persistence backend:

```javascript
import SQL from "/core/SQL.js"

const db = new SQL({ name: "akao" })
await db.ready   // wait for worker to open the DB

await db.exec(`CREATE TABLE IF NOT EXISTS orders (
    id      INTEGER PRIMARY KEY AUTOINCREMENT,
    userId  TEXT NOT NULL,
    total   REAL,
    status  TEXT DEFAULT 'pending'
)`)

const { lastId } = await db.run(
    "INSERT INTO orders (userId, total) VALUES (?, ?)",
    ["user-abc", 99.50]
)
const order   = await db.get("SELECT * FROM orders WHERE id = ?", [lastId])
const pending = await db.all("SELECT * FROM orders WHERE status = ?", ["pending"])

await db.batch([
    { sql: "UPDATE orders SET status = ? WHERE id = ?", params: ["paid", 1] },
    { sql: "INSERT INTO ledger (orderId, amount) VALUES (?, ?)", params: [1, 99.50] }
])
```

**Key design decisions:**
- `sql` worker runs SQLite in WAL mode with `wal_autocheckpoint=0` — periodic `wal_checkpoint(PASSIVE)` every 2 s prevents OPFS thrash
- All SQL calls go through `Threads.queue()` — same pattern as other workers
- `OPFS.js` is a standalone async OPFS wrapper (usable independently of SQLite)

See [docs/thoughts/sqlite-wasm-opfs-worker.md](docs/thoughts/sqlite-wasm-opfs-worker.md) for full architecture details.

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
│   └── SQL/       — sqlite3.js (WASM module) + sqlite3.wasm
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
- ZEN key pair derived deterministically from WebAuthn credential hash (no seed phrases)
- Public key storage in ZEN P2P graph
- Multi-wallet support (wallet ID selects BIP-32 derivation path)
- Avatar support with deterministic identicons

## 🗄️ Data Storage

| Layer | Module | Purpose |
|---|---|---|
| SQLite WASM | `SQL.js` | Full SQL queries, relational data, runs in dedicated worker via OPFS |
| IndexedDB | `IDB.js` | Client-side persistent cache |
| OPFS | `OPFS.js` | Origin Private File System — async file I/O (blobs, arbitrary files) |
| ZEN | `ZEN.js` | P2P graph database with built-in crypto (offline-first, realtime) |
| WebRTC | `RTC.js` | P2P DataChannel — signaling-agnostic offer/answer/accept |
| WebTorrent | `Torrent.js` | In-browser BitTorrent over WebRTC — seed and stream files |
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

The important rule is architectural, not just procedural: tests revolve around the generated `build/` runtime. The suite first verifies real build artifacts, then boots the real runtime through `Launcher.js` in Node headless mode and in the browser route `/test`.

### Running Tests

```bash
npm test              # same as test:core
npm run test:core     # build verification + browser runtime + Playwright assertions
npm run test:build    # verify generated build/ artifacts only
npm run test:browser  # build/core + Node headless browser-route runner
npm run test:playwright
npm run test:isomorphic
npm run test:geo      # build/geo integrity tests
```

For a live interactive view, open `/{locale}/test` in the dev server (for example `http://localhost:8080/en/test` or an alias host such as `http://peer0.akao.io:8080/en/test`). The browser UI shows pass/fail per suite in real time, lets you re-run individual suites or only failed tests, and displays inline error messages.

### Coverage Areas

The active suite spans:

- build artifact verification in `build/`
- Node headless boot through `build/core/Launcher.js`
- browser runtime boot through `/{locale}/test`
- core modules such as Events, States, Utils, Router, DB, Context, Access, RTC, Torrent, Order, Lock, and Trade
- interactive or browser-only cases surfaced in the live `/test` route when they are not suitable for automated headless runs

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
peers: []   # ZEN peers for decentralized sync
```

`src/statics/domains.yaml`:

```yaml
mystore.com: mystore
```

## 📚 Documentation

- [Tại Sao AKAO](docs/thoughts/akao.md) — triết lý và nguồn gốc
- [Philosophy Index](docs/phylosophy/README.md) — The worldview behind the codebase
- [The Philosophy of Framework-less](docs/thoughts/the-philosophy-of-framework-less.md) — Why no framework?
- [SQLite WASM + OPFS Architecture](docs/thoughts/sqlite-wasm-opfs-worker.md) — SQL storage design
- [Offline-First In-Browser Server](docs/thoughts/offline-first-browser-server.md) — RTC, Torrent, P2P stack architecture & feasibility
- [WebAuthn PRF Extension](docs/thoughts/webauthn-prf-extension.md) — Passkey deep-dive
- [White Paper](docs/thoughts/white-paper.md) — 4-party platform design
- [Chat](docs/thoughts/chat.md) — platform chat protocol

## 🤝 Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for:

- Development setup
- Code style guide
- Component creation guide
- Commit message format

## 📄 License

MIT License — see [LICENSE](LICENSE) for details.

---

**Built with ❤️ from 🇻🇳 Vietnam 🍜🐉🪷🏯🌸🪭💋**
