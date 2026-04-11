// Node.js dev shim — re-exports the ethers package.
// In production, the builder replaces this with node_modules/ethers/dist/ethers.min.js → build/core/Ethers.js
export * from "ethers"
