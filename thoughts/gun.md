# Gun OPFS notes

`Construct.GDB()` now prefers Gun's OPFS-backed store when the runtime supports it, and falls back to the existing IndexedDB-backed store otherwise.

## Load order

`src/core/GDB.js` imports Gun in this order:

1. `gun.js`
2. `sea.js`
3. `lib/radix.js`
4. `lib/radisk.js`
5. `lib/opfs.js`
6. `lib/rindexed.js`
7. `lib/store.js`
8. `lib/pen.js`

This matters because both `lib/opfs.js` and `lib/rindexed.js` attach a `Gun.on("create")` hook and may provide `root.opt.store`.

## Runtime behavior

In `src/core/Construct.js`, `Construct.GDB()` does this after importing `./GDB.js`:

- reads `globalThis.ROPFS`
- builds the normal Gun options:
  - `peers`
  - `localStorage: false`
  - `radisk: true`
  - `file: "GDB"`
- checks `opfs?.supported?.({ file })`
- when supported, sets `gunOptions.store = opfs({ file })`
- creates the Gun instance with `GUN(gunOptions)`

Because `root.opt.store` is already set in the OPFS case, Gun's IndexedDB adapter (`lib/rindexed.js`) leaves it alone. If OPFS is not supported, no explicit store is set, so `rindexed.js` behaves as before and provides the IndexedDB store.

## Fallback chain

1. **OPFS available** -> use Gun `lib/opfs.js`
2. **OPFS unavailable** -> fall back to Gun `lib/rindexed.js`
3. `lib/store.js` continues to use whichever `opt.store` was selected

## Build requirement

The core build must ship Gun's OPFS adapter. `builder/core.js` now copies:

- `node_modules/@akaoio/gun/lib/opfs.js`
- to `build/core/GDB/lib/opfs.js`

Without that copied file, the runtime import in `src/core/GDB.js` would fail in the built output.

## Why this approach

- keeps the current IndexedDB behavior unchanged for unsupported browsers
- enables OPFS automatically in browsers that expose `navigator.storage.getDirectory()`
- avoids changing Gun internals by selecting the store through normal Gun options
