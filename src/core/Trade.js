/**
 * Trade — central four-party protocol orchestrator.
 *
 * Coordinates cryptographic key derivation, state management, payload
 * encryption, and event routing for the E/B/S/A escrow protocol described
 * in docs/thoughts/white-paper.md.
 *
 * Roles:
 *   E — Escrow / Platform (trusted arbiter)
 *   B — Buyer
 *   S — Seller
 *   A — Affiliate
 *
 * This class is generic and protocol-agnostic: it provides primitives that
 * can power escrow, settlement, referral, chat, and dispute flows without
 * being hard-coded to any one workflow.
 *
 * Reuses:
 *   - sha256 from Utils/crypto.js
 *   - sea (GunDB SEA) from globalThis for ECDH / encrypt / sign
 *   - HDNodeWallet / getBytes from ethers for BIP-32 HD key derivation
 */

import { bind } from "./Trade/bind.js"
import { unbind } from "./Trade/unbind.js"
import { open } from "./Trade/open.js"
import { lock } from "./Trade/lock.js"
import { release } from "./Trade/release.js"
import { refund } from "./Trade/refund.js"
import { settle } from "./Trade/settle.js"
import { resolve } from "./Trade/resolve.js"
import { dispute } from "./Trade/dispute.js"
import { seed } from "./Trade/seed.js"
import { derive } from "./Trade/derive.js"
import { pair } from "./Trade/pair.js"
import { root } from "./Trade/root.js"
import { child } from "./Trade/child.js"
import { wrap } from "./Trade/wrap.js"
import { sign } from "./Trade/sign.js"
import { verify } from "./Trade/verify.js"
import { route } from "./Trade/route.js"
import { sync } from "./Trade/sync.js"
import { emit } from "./Trade/emit.js"
import { snapshot } from "./Trade/snapshot.js"
import { restore } from "./Trade/restore.js"
import { add } from "./Trade/add.js"
import { del } from "./Trade/del.js"
import { get } from "./Trade/get.js"
import { set } from "./Trade/set.js"

export class Trade {
    constructor(options = {}) {
        // Trade identifier (e.g. orderId or sessionId)
        this.id = options.id || ""

        // Protocol scope — domain label used as a derivation separator
        // (e.g. "item", "order", "session", "dispute")
        this.scope = options.scope || ""

        // Which role this instance is acting as (optional — affects default
        // signing / encryption when no role is explicitly passed to a method)
        this.role = options.role || ""

        // Role registry: role symbol → actor { pub, pair?, epub?, epriv? }
        this.roles = new Map()

        // Protocol state store: arbitrary key/value pairs
        // (status, itemId, orderId, amount, currency, etc.)
        this.state = new Map()

        // Derived key material cache: "A:B" → { secret, seed }
        // Avoids recomputing expensive ECDH on every call
        this.keys = new Map()

        // Event/message routing: event-name → Set<handler>
        this.handlers = new Map()
    }

    // ── Role management ───────────────────────────────────────────────────────
    bind = bind
    unbind = unbind

    // ── Protocol state transitions ────────────────────────────────────────────
    open = open
    lock = lock
    release = release
    refund = refund
    settle = settle
    resolve = resolve
    dispute = dispute

    // ── Key derivation (per white-paper §3) ──────────────────────────────────
    seed = seed
    pair = pair
    derive = derive
    root = root
    child = child

    // ── Payload cryptography ──────────────────────────────────────────────────
    wrap = wrap
    sign = sign
    verify = verify

    // ── Routing / sync / events ───────────────────────────────────────────────
    route = route
    emit = emit
    sync = sync

    // ── State snapshot / restore ──────────────────────────────────────────────
    snapshot = snapshot
    restore = restore

    // ── State CRUD ────────────────────────────────────────────────────────────
    add = add
    del = del
    get = get
    set = set
}

export default Trade
