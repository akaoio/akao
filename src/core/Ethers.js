// Development shim for Node.js (tests, build scripts).
// In the browser, build/core/Ethers.js is replaced by the bundled ethers.min.js
// copied from node_modules/ethers/dist/ethers.min.js during `npm run build:core`.
export * from "ethers"
