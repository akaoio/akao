# Hot Module Replacement

Development-mode hot module replacement system for Web Components with automatic state preservation.

## System Architecture

### Components

**Dev Server** (`dev.js`)
- File system monitoring via Chokidar
- Incremental build pipeline
- Server-Sent Events broadcast endpoint
- Early inline bootstrap injection before any ES modules run
- Dev-mode injection for alias hosts served by the dev server, not just `localhost`

**HMR Runtime** (`src/core/HMR.js`)
- Module registry with dependency tracking
- Custom element registration interception
- Dynamic import with cache invalidation
- Granular update handlers (CSS, template, module)

**HMR Client** (`src/core/HMR/client.js`)
- EventSource SSE connection with auto-reconnect
- Update message routing to runtime
- Automatic injection via dev server
- Shared `_dev` state object for connection/runtime diagnostics

### Update Pipeline

```
File Change → Chokidar → Incremental Build → SSE Broadcast
                                                     ↓
Browser ← Re-render Component ← Runtime Dispatch ← Client
```

## Update Strategies

### CSS Hot Injection
**File pattern:** `*.css.js`  
**Latency:** ~10ms  
**Method:** Direct Shadow DOM style element replacement  
**State impact:** None

### Template Re-render
**File pattern:** `template.js`  
**Latency:** ~30-50ms  
**Method:** Dynamic import with versioned URL, `render()` invocation on live instances  
**State impact:** Component state preserved via `States` proxy

### Module Hot Swap
**File pattern:** `index.js`  
**Latency:** ~100-200ms  
**Method:** Re-import module, bind new prototype methods to existing instances  
**State impact:** Component state preserved, instance references maintained

### Full Page Reload
**File pattern:** Core modules, global config  
**Trigger:** `src/index.html`, `src/statics/{locales,system,domains}.yaml`  
**Fallback:** Syntax errors, import failures

## Implementation Details

### Custom Element Tracking

The HMR runtime intercepts `customElements.define()` to maintain a registry mapping tag names to:
- Component class constructor
- Source module URL (derived from stack trace)

Element instances are located via `document.querySelectorAll(tagName)` during updates to avoid WeakSet iteration limitations.

### Cache Invalidation

Module re-imports append a timestamp query parameter (`?v=${Date.now()}`) to bypass browser module cache:

```javascript
const versionedUrl = `${url}?v=${timestamp}`
const module = await import(versionedUrl)
```

### State Preservation

Component state is preserved through the `States` reactive proxy. During hot swap:

```javascript
const preservedStates = element.states?.states || {}
// ... hot swap logic ...
element.states.set(preservedStates)
```

Global state (`Context`, `Access`, `Cart`) remains in memory as module-level singletons are not re-executed.

## Usage

### Automatic Initialization

HMR activates automatically on pages served by the dev server (`npm start`). That includes `localhost` and any alias host that the dev server injects into dev mode.

```
🔥 HMR: Runtime initialized
🔌 HMR: Connected to dev server
```

### Development Cycle

Edit source files in `src/`. Changes are detected, rebuilt incrementally, and broadcast to connected clients. Updates apply without page reload or state loss.

Console logs indicate update status:

```
🔥 HMR: template update for /UI/components/button/template.js
✅ HMR: Template updated
```


## Debugging

### Runtime State Inspection

```javascript
// Dev/HMR connection state
window._dev
// { enabled, connectedAt, lastMessageAt, messageCount, readyState, hmrEnabled, ... }

// Runtime instance
window.hmr

// Module registry
window.hmr.modules
// Map<URL, { exports, timestamp, dependents, dependencies }>

// Element registry
window.hmr.elements
// Map<tagName, { class, module }>
```

### Manual Reload

```javascript
window.location.reload()
```

## Constraints

### Full Reload Triggers

The following file changes require full page reload:
- `src/index.html`
- `src/statics/locales.yaml`
- `src/statics/system.yaml`
- `src/statics/domains.yaml`
- `src/statics/chains/**`
- `src/statics/ABIs/**`

### Non-Compatible Patterns

Module-level side effects prevent HMR. Move initialization to component lifecycle:

```javascript
// ❌ Incompatible: module-level side effect
const ws = new WebSocket("wss://...")

// ✅ Compatible: lifecycle-bound
export class COMPONENT extends HTMLElement {
  connectedCallback() {
    this.ws = new WebSocket("wss://...")
  }
  disconnectedCallback() {
    this.ws?.close()
  }
}
```

## Performance Characteristics

| Update Type      | Latency   | Network | State Impact |
|------------------|-----------|---------|--------------|
| CSS              | 10-20ms   | None    | Preserved    |
| Template         | 30-50ms   | None    | Preserved    |
| Component logic  | 100-200ms | None    | Preserved    |
| Full reload      | 1-3s      | Full    | Lost         |

HMR provides 10-100x faster iteration cycles compared to full page reload.

## Technical Implementation

### Component Registry

Custom element definitions are intercepted via `customElements.define()` override. Metadata stored includes:
- Component class constructor
- Source module URL (extracted via stack trace analysis)

Live element instances are located using `document.querySelectorAll(tagName)` during hot updates.

### Update Message Protocol

Server broadcasts JSON messages via SSE:

```json
{
  "type": "hmr",
  "path": "/UI/components/button/template.js",
  "updateType": "template",
  "timestamp": 1234567890
}
```

### File Structure

```
src/core/HMR.js           Runtime engine
src/core/HMR/client.js    SSE client (auto-injected)
dev.js                    Dev server with broadcast logic
```

## Notes on Current Behavior

- Dev mode is no longer inferred from `localhost` alone. The dev server injects `_dev.enabled = true` into served HTML, and the runtime respects that on alias hosts too.
- `_dev` is a shared state object, not a boolean flag. HMR client/runtime both write into this object.
- `window.hmr` is the canonical runtime/debug surface. Older examples using `window.__hmr` or `window.__devSseState` are stale.
