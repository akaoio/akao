import { Chains } from "../Stores.js"

/**
 * Transition the trade to "refunded" status (escrow funds returned to buyer).
 * When a chain is configured on the Trade instance and opts provides the escrow
 * spending key, executes the on-chain refund transaction via Chains[chain].send().
 * All opts keys (except key) are also merged into the trade's state.
 * @param {Object} [opts] - State data and optional on-chain refund parameters
 * @param {string} [opts.key] - Escrow wallet private key (VSE.privateKey)
 * @param {string} [opts.from] - Escrow wallet address (VSE.address)
 * @param {string} [opts.to] - Recipient address (buyer)
 * @param {string} [opts.amount] - Amount to refund
 * @param {*} [opts.currency] - Currency descriptor passed to chain.send()
 * @returns {Promise<Trade>} this (chainable)
 */
export async function refund(opts = {}) {
    // Set state synchronously so callers don't need to await for status checks
    this.state.set("status", "refunded")
    if (opts && typeof opts === "object")
        for (const [k, v] of Object.entries(opts)) this.state.set(k, v)

    // Execute on-chain refund when chain and spending key are available
    const chain = this.chain ? Chains[this.chain] : null
    if (chain && opts?.key && opts?.from && opts?.to && opts?.amount) {
        try {
            await chain.send({ from: opts.from, to: opts.to, amount: opts.amount, currency: opts.currency, key: opts.key })
        } catch (error) {
            this.state.set("status", "failed")
            this.state.set("error", error?.message || String(error))
            return this
        }
    }

    return this
}
