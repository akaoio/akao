# P2P Trading Escrow Protocol

**Version** 2.0 · **Status** Production · **Date** 2026-04-06

> **Supersedes**: v1.0 eCommerce model (deprecated - see `white-paper-v1-ecommerce-deprecated.md`)
>
> **Philosophy**: DEX-inspired P2P trading where all participants are symmetric traders. Platform provides infrastructure, not custody.

---

## Abstract

This document specifies a **trustless, serverless escrow protocol for peer-to-peer trading** of digital assets (game items, NFTs, digital goods). The protocol enables a platform operator to mediate trades between two traders (maker and taker) using deterministic cryptographic key derivation — eliminating the need for smart contracts, custodial accounts, or trusted third-party oracles.

All escrow wallet addresses are derived client-side from **Diffie-Hellman shared secrets** and **BIP-32 hierarchical deterministic key derivation**, ensuring that each party holds exactly the access rights the protocol intends, and no more.

### Think DEX, Not Marketplace

**Traditional marketplace** (v1.0 - deprecated):
- ❌ Fixed roles: Buyer pays, Seller sells
- ❌ Item-centric: Each item has separate escrow tree
- ❌ One-directional: Only buyers initiate trades
- ❌ Custodial mindset: Platform holds listings

**DEX/Trading mindset** (v2.0 - current):
- ✅ **Symmetric roles**: Anyone can create buy OR sell orders
- ✅ **Trader-centric**: Maker/Taker like Uniswap, dYdX, Binance
- ✅ **Bi-directional**: Orders are just intentions (buy intention = sell intention from other side)
- ✅ **Order book**: Decentralized discovery via Gun + Pen validation
- ✅ **Affiliate economy**: Built-in referral commissions
- ✅ **Non-custodial (happy path)**: Platform arbitrates disputes/refunds, not involved in normal trades

**Key innovations**:
- **Dual escrow wallets** per trade: Payment Lock (PL) + Affiliate Lock (AL)
- **Temporal validation**: Orders auto-expire using candle-based epochs (Pen DSL)
- **Post-match deposits**: Both order types deposit after matching (pure P2P, no Platform involvement)
- **Trustless verification**: Anyone can verify escrow addresses on-chain before accepting

---

## 1. Roles and Trust Model

### 1.1 Parties

| Role | Symbol | Description |
|---|---|---|
| **Platform** | **P** | Marketplace operator. Holds authority to release or refund escrow funds. Acts as trusted arbiter in disputes. |
| **Maker** | **M** | Trader who creates an order (buy or sell). |
| **Taker** | **T** | Trader who accepts/matches an existing order. |
| **Affiliate** | **A** | Referrer who brought the payer to the platform. Earns commission on successful trades. |

**Note:** M and T are symmetric — both are traders. A trader can be M in one trade and T in another. The distinction only matters during a specific trade execution. A is optional — if payer has no referrer, no affiliate commission is paid.

### 1.2 Escrow Wallets

| Wallet | Symbol | Full Name | Purpose | Derived From |
|--------|--------|-----------|---------|--------------|
| Payment escrow | **PL** | Payment Lock | Holds trade payment (payer → recipient) | `root_MP` or `root_TP` |
| Commission escrow | **AL** | Affiliate Lock | Holds affiliate commission (payer → affiliate) | `root_AP` |

**Note:** Each trade creates exactly 2 lock wallets: 1 PL (payment) + 1 AL (commission, if affiliate exists).

### Trust Assumptions

- **P is trusted** to act honestly as arbiter (semi-centralized model)
- **No party other than P** can unilaterally move funds from escrow before trade resolution
- **Both traders trust P** to resolve disputes fairly
- All parties have valid **SEA key pairs** registered via WebAuthn

---

## 2. Cryptographic Primitives

Same as v1.0 (no changes):

### 2.1 ECDH Shared Secret — `sea.secret`

`sea.secret(theirPublicKey, myKeyPair)` over Curve25519 (GunDB SEA library).

**Properties:**
- **Symmetric:** `sea.secret(P.pub, M.pair) === sea.secret(M.pub, P.pair)`
- **Deterministic:** Same inputs → same output
- **Pair-scoped:** `secret_MP ≠ secret_TP`

```javascript
const secret_MP = await sea.secret(P.pub, M.pair)  // M computes
const secret_MP = await sea.secret(M.pub, P.pair)  // P computes (same result)
```

### 2.2 BIP-32 Hierarchical Deterministic Wallets

From `ethers.js`:

```javascript
import { HDNodeWallet, getBytes } from "/core/Ethers.js"

const root = HDNodeWallet.fromSeed(getBytes("0x" + seedHex))
const xpub = root.neuter().extendedKey     // watch-only
const child = root.deriveChild(index)       // non-hardened derivation
```

**Critical property:** Anyone with `xpub` can derive child addresses, but only the holder of `xprv` can spend.

### 2.3 SHA-256 — `sha256`

From `Utils/crypto.js`, returns 64-char hex (32 bytes).

---

## 3. Order Types and Lifecycle

### 3.1 Order Types

**Buy Order:**
```javascript
{
    type: "buy",
    trader: "maker_pub_key",
    item: "diablo-4/penitent-greaves-d76bc",
    quantity: 1,
    price: 100,        // willing to pay
    currency: "USDT",
    chain: 1,          // Ethereum
    status: "open"
}
```

**Sell Order:**
```javascript
{
    type: "sell",
    trader: "maker_pub_key",
    item: "diablo-4/penitent-greaves-d76bc",
    quantity: 1,
    price: 95,         // willing to accept
    currency: "USDT",
    chain: 1,
    status: "open"
}
```

### 3.2 Trade Execution Flow

```
1. Maker creates order → writes to Gun with Pen-validated key (no deposit)
2. Taker discovers order → Gun .map() query
3. Taker accepts order → trade status: "open" → "matched" (both parties known)
4. Payer deposits to escrow (Maker for buy, Taker for sell):
   - Computes PL/AL from recipient's xpub + payer's index
   - Deposits within 10-min timeout or trade auto-cancels
5. In-game item delivery (trade/mail/drop)
6. Buyer confirms → reveals index to seller/affiliate
7. Recipients withdraw using own root + revealed index
8. Trade status: "matched" → "deposited" → "delivered" → "completed"
```

### 3.3 Key Derivation Strategy

**Changes from v1.0:**
- **No per-item roots** (items don't need separate xpub trees)
- **One root per trader-platform pair**
- **Two escrow wallets per trade:**
  - **Payment escrow (PL)** — holds trade payment (M/P or T/P root)
  - **Affiliate escrow (AL)** — holds affiliate commission (A/P root)

---

## 4. Key Derivation Specification

### 4.1 Notation

| Symbol | Description |
|---|---|
| `⊕` | String concatenation with `":"` separator |
| `sha256(x)` | SHA-256 hex (64 chars) |
| `fromSeed(h)` | `HDNodeWallet.fromSeed(getBytes("0x" + h))` |
| `index(h)` | `parseInt(h.substring(0, 8), 16) & 0x7fffffff` (31-bit non-hardened) |

### 4.2 Root Key for Each Trader/Affiliate

Each party derives a single root HD key with Platform:

```javascript
secret_MP  =  sea.secret(P.pub, M.pair)
secret_TP  =  sea.secret(P.pub, T.pair)
secret_AP  =  sea.secret(P.pub, A.pair)

seed_MP    =  sha256(secret_MP)
seed_TP    =  sha256(secret_TP)
seed_AP    =  sha256(secret_AP)

root_MP    =  fromSeed(seed_MP)    // full node — known to M and P
root_TP    =  fromSeed(seed_TP)    // full node — known to T and P
root_AP    =  fromSeed(seed_AP)    // full node — known to A and P
```

**Each party publishes their xpub:**
- Maker: `xpub_MP = root_MP.neuter().extendedKey` (in user profile on Gun)
- Taker: `xpub_TP = root_TP.neuter().extendedKey`
- Affiliate: `xpub_AP = root_AP.neuter().extendedKey` (in referral link)

### 4.3 Per-Payment Lock Wallets (Dual Escrow)

**Key principle from original design**: Escrow wallet = **RECIPIENT's root xpub** + **INDEX from PAYER's secret**

**Critical design decision**: Both buy and sell orders deposit **AFTER matching** to preserve pure P2P architecture (Platform not involved in happy path).

For each trade, derive **two escrow wallets**:

#### 4.3.1 Payment Lock (PL)

**Symmetric flow for both order types** (Maker always uses Taker's xpub after match):

```javascript
// AFTER MATCHING (both parties known)
tradeId = sha256(orderId + M.pub + T.pub + timestamp)

// Step 1: Recipient (Taker for buy order, Maker for sell order) publishes xpub
secret_recipient = sea.secret(P.pub, recipient.pair)
root_recipient = fromSeed(sha256(secret_recipient))
xpub_recipient = root_recipient.neuter().extendedKey  // Public

// Step 2: Payer (Maker for buy order, Taker for sell order) computes index
secret_payer = sea.secret(P.pub, payer.pair)
seed_index = sha256(secret_payer + ":" + tradeId)
index_PL = parseInt(seed_index.slice(0,8), 16) & 0x7fffffff

// Step 3: Payer derives escrow from RECIPIENT's xpub + PAYER's index
PL = HDNodeWallet.fromExtendedKey(xpub_recipient).deriveChild(index_PL)
//   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Recipient's root (watch-only)
//                                               ^^^^^^^^^ Payer's index

// PL.address — Payer deposits HERE (after matching)
// PL.privateKey — Payer doesn't have (only has xpub, not xprv)
// Recipient needs index_PL revealed to derive spending key
```

**Who is payer/recipient?**
- **Buy order**: Maker (buyer) is payer, Taker (seller) is recipient
- **Sell order**: Taker (buyer) is payer, Maker (seller) is recipient

#### 4.3.2 Affiliate Lock (AL)

**Same pattern - payer uses affiliate's xpub + payer's index:**

```javascript
// AFTER MATCHING (payer known)
tradeId = sha256(orderId + M.pub + T.pub + timestamp)

// Affiliate publishes xpub
secret_AP = sea.secret(P.pub, A.pair)
root_AP = fromSeed(sha256(secret_AP))
xpub_AP = root_AP.neuter().extendedKey

// Payer computes index from own secret
secret_payer = sea.secret(P.pub, payer.pair)
seed_index = sha256(secret_payer + ":" + tradeId)
index_AL = parseInt(seed_index.slice(0,8), 16) & 0x7fffffff

// Payer derives from AFFILIATE's xpub + PAYER's index
AL = HDNodeWallet.fromExtendedKey(xpub_AP).deriveChild(index_AL)

// AL.address — payer deposits commission here (after matching)
// AL.privateKey — Affiliate needs index revealed to spend
```

**Security properties**:
- Payer derives escrow address from **recipient's xpub** (watch-only)
- Payer uses index from **own secret** (knows index)
- Payer **cannot spend** (doesn't have recipient's xprv, only xpub)
- Recipient **cannot spend yet** (doesn't know index)
- Platform **can always spend** (knows all secrets → can compute all indexes + has all xprvs)

**Who pays (AFTER matching):**
- **Buy order:** Maker deposits to `PL` + `AL` (both use Taker's/Affiliate's xpub + Maker's index)
- **Sell order:** Taker deposits to `PL` + `AL` (both use Maker's/Affiliate's xpub + Taker's index)

**When deposit happens:**
- **Both order types**: Deposit occurs AFTER matching (when both parties known)
- No pre-deposit required → Platform not involved in happy path → pure P2P

**Access control:**
- **Payer (M or T):** Computes escrow addresses from recipient's xpub + own index. **Cannot spend** (has xpub only, not xprv).
- **Recipient (seller):** 
  - Buy order: Taker receives `index_PL` from Maker → derives spending key from own `root_TP`
  - Sell order: Maker receives `index_PL` from Taker → derives spending key from own `root_MP`
- **Affiliate (A):** Receives `index_AL` from payer (or Platform) → derives spending key from own `root_AP`
- **Platform (P):** Knows all secrets → can compute all indexes + derive all spending keys (arbitration power)

**Unlock mechanism**:
```
BOTH ORDER TYPES FOLLOW SAME FLOW (symmetric):

1. Maker posts order (no deposit)
2. Taker accepts → tradeId created
3. Payer computes PL/AL from Recipient's xpub + Payer's index
4. Payer deposits to PL.address + AL.address (within deposit timeout)
5. Seller delivers item
6. Buyer confirms receipt → reveals index_PL and index_AL
7. Recipient uses own root + revealed index → derives spending key → withdraws
```

**Why payer cannot withdraw**:
- Payer only has recipient's **xpub** (public extended key, watch-only)
- Even though payer knows the **index**, cannot derive private key without xprv
- BIP-32 non-hardened derivation: `xpub + index → child address` ✅ (watch-only)
- But: `xpub + index → child private key` ❌ (impossible, needs xprv)

**Why recipient cannot withdraw early**:
- Recipient has **root xprv** (own wallet)
- But doesn't know which **index** was used (payer's secret + entropy)
- Without index, 2^31 possible child keys to try (infeasible)

**Original security mindset preserved**: Escrow wallet = recipient's root + payer's index

---

## 5. Protocol Walkthrough

### Step 1 — Maker Creates Order

**Both order types follow same pattern** (symmetric design):

1. M creates order on UI (buy or sell)
2. M generates `orderId = sha256(M.pub ⊕ item ⊕ price ⊕ timestamp)`
3. M retrieves referrer (if any) from localStorage: `Context.getReferrer()`
4. M writes order to Gun with Pen-validated key:
   ```
   <candle>:<item_slug>:<type>:<nonce>
   
   Example buy:  5820000:penitent-greaves:buy:a3f7b2
   Example sell: 5820000:penitent-greaves:sell:x8k2m1
   ```
5. Order includes metadata: `{ orderId, type, item, price, currency, chain, referrer }`
6. Order status: `"open"` (no funds locked yet, waiting for Taker)

**No pre-deposit** for either order type → pure P2P (Platform not involved)

### Step 2 — Taker Discovers and Accepts Order

1. T queries Gun order book:
   ```javascript
   gun.get(orderSoul).map({
       '.': { '*': `${candle}:penitent-greaves:buy:` }
   })
   ```
2. T sees M's order, clicks "Accept"
3. **Trade matching** (creates `tradeId`):
   ```javascript
   tradeId = sha256(orderId + M.pub + T.pub + timestamp)
   ```
4. Trade status → `"matched"` (both parties known, waiting for deposit)

### Step 3 — Payer Deposits to Escrow

**Payer** (Maker for buy order, Taker for sell order) now computes escrow addresses:

```javascript
// Determine payer and recipient based on order type
const payer = (orderType === 'buy') ? Maker : Taker
const recipient = (orderType === 'buy') ? Taker : Maker

// Get recipient's xpub (published in their profile)
const xpub_recipient = await gun.user(recipient.pub).get('xpub')

// Get affiliate's xpub (if referrer exists)
const referrer = payer.getReferrer()
const xpub_affiliate = referrer ? await gun.user(referrer).get('xpub') : null

// Compute payer's index
const secret_payer = await sea.secret(P.pub, payer.pair)
const seed_index = sha256(secret_payer + ":" + tradeId)
const index_PL = parseInt(seed_index.slice(0,8), 16) & 0x7fffffff

// Derive Payment Lock from recipient's xpub + payer's index
const PL = HDNodeWallet.fromExtendedKey(xpub_recipient).deriveChild(index_PL)

// Derive Affiliate Lock (if affiliate exists)
const AL = referrer ? 
    HDNodeWallet.fromExtendedKey(xpub_affiliate).deriveChild(index_PL) : 
    null

// Payer deposits
await payer.wallet.sendTransaction({ to: PL.address, value: paymentAmount })
if (AL) {
    await payer.wallet.sendTransaction({ to: AL.address, value: commissionAmount })
}
await payer.wallet.sendTransaction({ 
    to: Platform.operationalWallet, 
    value: platformFee 
})
```

**Deposit timeout**: If payer doesn't deposit within 10 minutes → trade auto-cancels → order returns to "open"

**After confirmations**: Trade status → `"deposited"` (funds locked)

### Step 4 — Item Delivery

**Seller** (Taker for buy order, Maker for sell order) delivers in-game:
- Trade window in-game
- In-game mail system
- Drop/pickup coordination

**Seller marks:** "Delivered" in UI → updates Gun trade record → status: `"delivered"`

### Step 5 — Buyer Confirms Receipt and Unlocks Payment

**Buyer** (Maker for buy order, Taker for sell order) marks "Received" in UI

**Buyer reveals index to seller** (symmetric for both order types):

```javascript
// Buyer computes index from own secret
const secret_buyer = await sea.secret(P.pub, buyer.pair)
const index_PL = index(sha256(secret_buyer + ":" + tradeId))

// Buyer transmits via Gun
gun.get(tradeRecordSoul).put({ 
    unlock_index_PL: index_PL,
    unlock_index_AL: index_PL,  // Same index for affiliate
    status: 'completed'
})

// Seller receives index and unlocks using OWN root
const secret_seller = await sea.secret(P.pub, seller.pair)
const root_seller = HDNodeWallet.fromSeed(sha256(secret_seller))
const PL = root_seller.deriveChild(index_PL)  // Now has private key!

// Seller self-releases payment
await PL.sendTransaction({
    to: seller.wallet,
    value: paymentAmount
})

// Affiliate (if exists) self-releases commission
if (affiliate) {
    const secret_affiliate = await sea.secret(P.pub, affiliate.pair)
    const root_affiliate = HDNodeWallet.fromSeed(sha256(secret_affiliate))
    const AL = root_affiliate.deriveChild(index_PL)
    
    await AL.sendTransaction({
        to: affiliate.wallet,
        value: commissionAmount
    })
}
```

Trade status → `"completed"`

**OR via Platform auto-release:**
- If buyer doesn't confirm within 24h → P computes indexes and releases to seller + affiliate

### Step 6 — Dispute Resolution and Refunds by P

**Critical limitation**: Refunds REQUIRE Platform involvement due to escrow design.

**Why payer cannot self-refund:**
```javascript
// Escrow = Recipient's xpub + Payer's index
PL = HDNodeWallet.fromExtendedKey(xpub_recipient).deriveChild(index_payer)

// Payer has:
// - Recipient's xpub (watch-only, public)
// - Own index (secret)
// BUT: Cannot derive private key from xpub alone!

// Only recipient has xprv → only recipient can spend
// Payer is cryptographically locked out (by design)
```

**This means:**
- ✅ Happy path (delivery + confirmation): Pure P2P, Platform not involved
- ⚠️ **Unhappy path (timeouts/disputes): Platform MUST intervene to refund**
- This is **not** centralization — Platform acts as trusted arbiter (per Section 1.1)

**Platform-initiated refund mechanism:**

P can always recompute both escrow spending keys:

```javascript
// P knows all parties' pub keys
const secret_recipient = await sea.secret(recipient.pub, P.pair)
const root_recipient = fromSeed(sha256(secret_recipient))

// P can compute payer's index (knows both secrets)
const secret_payer = await sea.secret(payer.pub, P.pair)
const index_PL = index(sha256(secret_payer + ":" + tradeId))

// P derives escrow spending key
const PL = root_recipient.deriveChild(index_PL)
// PL.privateKey → P can release to seller OR refund to payer

// Same for affiliate lock
const secret_affiliate = await sea.secret(affiliate.pub, P.pair)
const root_affiliate = fromSeed(sha256(secret_affiliate))
const AL = root_affiliate.deriveChild(index_PL)
// AL.privateKey → P can release to affiliate OR refund to payer
```

**Refund triggers** (Platform monitors for these conditions):

1. **Payer requests refund** (after timeout)
   - Payer submits refund request via UI
   - Platform verifies timeout conditions met
   - Platform executes refund

2. **Automatic timeout** (optional - Platform worker)
   - Platform's `update.js` thread polls trades
   - If trade >24h in "deposited" state with no delivery confirmation
   - Platform auto-refunds to payer

3. **Dispute resolution**
   - Either party files dispute via UI
   - Platform investigates (chat logs, screenshots, etc.)
   - Platform decides: refund to payer OR release to seller

**Resolution matrix:**

| Scenario | PL (Payment) Action | AL (Commission) Action |
|---|---|---|
| Trade completed successfully | Buyer reveals index → Seller withdraws | Buyer reveals index → Affiliate withdraws |
| Buyer disputes, seller proven wrong | Platform refunds to payer (transfers) | Platform refunds to payer (transfers) |
| Buyer disputes, seller proven right | Platform reveals index to seller → Seller withdraws | Platform reveals index to affiliate → Affiliate withdraws |
| Both parties inactive 7+ days | Platform refunds to payer (transfers) | Platform refunds to payer (transfers) |
| Auto-release (24h no confirmation) | Platform reveals index to seller → Seller withdraws | Platform reveals index to affiliate → Affiliate withdraws |

**Note**: Platform can either:
- **Reveal index** (seller/affiliate self-withdraws) — preferred for transparency
- **Transfer directly** (Platform executes withdrawal) — faster but less transparent

---

## 6. Order Book with Pen DSL

### 6.1 Temporal Validation

Orders use **candle-based expiry** to prevent stale orders:

```javascript
const orderSoul = SEA.pen({
    key: { and: [
        // Candle window: current ± 100 candles (5-min candles = ~8 hours)
        SEA.candle({ seg: 0, sep: ":", size: 300000, back: 100, fwd: 2 }),
        
        // Item slug validation
        { seg: { sep: ":", idx: 1, of: { reg: 0 },
                 match: { length: [1, 128] } } },
        
        // Order type: buy or sell
        { seg: { sep: ":", idx: 2, of: { reg: 0 },
                 match: { or: [{ eq: "buy" }, { eq: "sell" }] } } }
    ]},
    val: { type: "string" },  // JSON order metadata (includes price)
    sign: true,               // Require signature
    pow: { field: 0, difficulty: 2 }  // Anti-spam PoW (field 0 = key, client iterates nonce at seg 3)
})
```

### 6.2 Key Format

```
<candle>:<item_slug>:<type>:<nonce>

Example buy:  5820000:penitent-greaves:buy:a3f7b2
Example sell: 5820000:penitent-greaves:sell:x8k2m1
```

**Note**: Price is stored in order metadata (value), not in key. This allows:
- Orders with same item/type to be grouped
- Price changes without key recreation
- Simpler LEX range queries by time (candle-first)

**Candle calculation:**
```javascript
const candle = Math.floor(Date.now() / 300000)  // 5-minute candles
```

**Benefits:**
- Old orders auto-expire (can't write keys outside window)
- Discovery queries scoped to recent candles
- No manual cleanup needed

---

## 7. Security Analysis

### 7.1 The Refund Limitation

**Critical design constraint**: Payer cannot self-refund in the current escrow model.

**Why this limitation exists:**

The escrow security relies on **asymmetric key access**:
```
Escrow = Recipient's xpub (watch-only) + Payer's index (secret)

Payer has:
├─ Recipient's xpub (public extended key)
└─ Own index (secret)

Recipient has:
├─ Own xprv (private extended key)
└─ Unknown index (until reveal)

Platform has:
├─ Recipient's xprv (via shared secret)
└─ Payer's index (via shared secret)
```

**BIP-32 cryptographic property**:
- `xpub + index → child address` ✅ (anyone can compute)
- `xprv + index → child private key` ✅ (only xprv holder can derive)
- `xpub + index → child private key` ❌ **IMPOSSIBLE** (by design)

**This means:**
- ✅ **Happy path**: Payer deposits → Recipient delivers → Payer reveals index → Recipient withdraws (pure P2P, Platform not involved)
- ⚠️ **Unhappy path**: Payer deposits → Recipient ghosts → Payer wants refund → **Platform MUST intervene** (Payer cannot derive private key from xpub alone)

**Trade-off accepted:**
- Security against payer withdrawal (prevents rug pulls)
- Cost: Refunds require Platform arbitration

**Alternative (not implemented):**
- Use smart contracts with time-locks (enables self-refund after timeout)
- Cost: Adds smart contract dependency, gas fees, complexity

**Current design philosophy**: Accept Platform as trusted arbiter for exceptional cases (disputes, timeouts) to maintain pure P2P for normal trades.

### 7.2 Access Control Guarantees

| Property | Mechanism |
|---|---|
| P holds full escrow authority | P knows all pub keys → recomputes all DH secrets → derives all root xprv and child spending keys for both PL and AL |
| Payer cannot withdraw (even for refund) | Payer has recipient's xpub only (watch-only). Cannot derive private key from xpub. **Refunds require Platform intervention.** |
| Recipient cannot claim early | Recipient has xprv but doesn't know index until payer reveals it |
| Affiliate cannot claim prematurely | A has xpub_AP but not `index_AL`; receives index only after successful trade completion |
| No third-party involvement (happy path) | All derivations local and deterministic. Platform only involved for disputes/refunds. |

### 7.3 Per-Trade Address Isolation

Each `tradeId` is unique → `index_PL` and `index_AL` unique → escrow addresses never collide across trades.

Even if same affiliate refers multiple trades, each trade gets unique AL wallet (different `tradeId` → different `index_AL`).

### 7.4 Order Book Spam Prevention

**Pen DSL enforces:**
- **Signature requirement** → only authenticated users
- **PoW (difficulty 2)** → CPU cost to create order
- **Temporal window** → old orders auto-expire

**Gun layer enforces:**
- **LEX prefix matching** → efficient discovery queries
- **P2P sync** → distributed order book

---

## 8. Risk Register

| Risk | Severity | Mitigation |
|---|---|---|
| `sea.secret` non-determinism | Critical | Test suite verification |
| SEA not initialized | High | Guard all operations; wait for `Construct.GDB()` |
| BIP-32 index overflow | Low | Masked to 31 bits via `& 0x7fffffff` |
| xpub tampering in Gun | Medium | Traders verify escrow address on-chain before deposit |
| Order ID collision | Very low | `sha256(orderId ⊕ M.pub ⊕ T.pub ⊕ timestamp)` |
| P key compromise | Critical | Operational security (outside protocol scope) |
| Payer cannot self-refund | **Design limitation** | **Platform MUST intervene for refunds.** Payer only has recipient's xpub (watch-only), cannot derive private key. This is by design — same mechanism that prevents payer from withdrawing prevents self-refund. |
| Platform unavailable (refunds blocked) | Medium | Multi-sig Platform keys, backup arbitrators, or smart contract fallback (future) |
| In-game item delivery fraud | Medium | Dispute resolution by P with proof requirements |
| Candle drift (client time skew) | Low | Accept ±2 candles forward, ±100 backward |

---

## 9. Out of Scope

- **Fiat payments** (handled by platform account balances)
- **Smart contract enforcement** (intentionally avoided)
- **Multi-signature** (future revision candidate)
- **Cross-chain atomic swaps** (single-chain trades only)
- **In-game API integration** (manual delivery via trade/mail)

---

## 10. Implementation Checklist

- [ ] `src/core/Escrow.js` — key derivation functions (PL + AL)
- [ ] `src/core/Order.js` — Gun order CRUD helpers
- [ ] `src/core/Affiliate.js` — referral tracking + commission calculation
- [ ] `src/UI/routes/order/` — order book UI + trade flow
- [ ] Pen soul definition for orders
- [ ] Gun .map() discovery queries
- [ ] Auto-release 24h timer (both PL and AL)
- [ ] Affiliate index release mechanism (via Gun)
- [ ] Dispute UI at `/dispute`
- [ ] Test suite: dual escrow address derivation consistency
- [ ] Test suite: `sea.secret` determinism
- [ ] Test suite: candle window validation
- [ ] Test suite: affiliate commission calculation

---

*2026-04-04 — Revised for P2P trading model*  
*2026-04-06 — Resolved: No pre-deposit for buy orders (pure P2P, symmetric design)*
