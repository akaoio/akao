import { Wallets } from "../Stores.js"

/**
 * Transition the trade to "locked" status (funds sent to escrow by buyer).
 * When a chain is configured on the Trade instance and a wallet for that chain
 * exists in the Wallets store, executes the on-chain payments:
 *   - platform fee → opts.platform
 *   - affiliate commission → opts.affiliate
 *   - item escrow → opts.escrow
 * All opts keys are also merged into the trade's state.
 * @param {Object} [opts] - State data and optional on-chain payment targets
 * @param {string} [opts.escrow] - Escrow wallet address (VSE.address)
 * @param {string} [opts.affiliate] - Affiliate wallet address (VAE.address)
 * @param {string} [opts.platform] - Platform fee address (E operational address)
 * @param {string} [opts.amount] - Item escrow amount
 * @param {string} [opts.affiliateAmount] - Affiliate commission amount
 * @param {string} [opts.platformAmount] - Platform fee amount
 * @param {*} [opts.currency] - Currency descriptor passed to wallet.send()
 * @returns {Promise<Trade>} this (chainable)
 */
export async function lock(opts = {}) {
    // Set state synchronously so callers don't need to await for status checks
    this.state.set("status", "locked")
    if (opts && typeof opts === "object")
        for (const [k, v] of Object.entries(opts)) this.state.set(k, v)

    // Execute on-chain payments when wallet and escrow address are available
    const wallet = this.chain ? Wallets[this.chain] : null
    if (wallet) {
        const escrow = opts?.escrow
        const amount = opts?.amount
        const currency = opts?.currency
        const affiliate = opts?.affiliate
        const affiliateAmount = opts?.affiliateAmount
        const platform = opts?.platform
        const platformAmount = opts?.platformAmount
        if (escrow && amount) {
            try {
                if (platform && platformAmount) await wallet.send({ to: platform, amount: platformAmount, currency })
                if (affiliate && affiliateAmount) await wallet.send({ to: affiliate, amount: affiliateAmount, currency })
                await wallet.send({ to: escrow, amount, currency })
            } catch (error) {
                this.state.set("status", "failed")
                this.state.set("error", error?.message || String(error))
                return this
            }
        }
    }

    return this
}
