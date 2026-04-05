# Trustless P2P Trading Escrow Protocol

**Version** 2.0 · **Status** Draft · **Date** 2026-04-04

---

## Abstract

This document specifies a **trustless, serverless escrow protocol for peer-to-peer trading** of digital game items. The protocol enables a platform operator to mediate trades between two parties (order maker and taker) using deterministic cryptographic key derivation — eliminating the need for smart contracts, custodial accounts, or trusted third-party oracles.

All escrow wallet addresses are derived client-side from **Diffie-Hellman shared secrets** and **BIP-32 hierarchical deterministic key derivation**, ensuring that each party holds exactly the access rights the protocol intends, and no more.

**Key differences from v1.0 (eCommerce model):**
- **Symmetric roles:** Both traders can create buy OR sell orders (no fixed buyer/seller)
- **Dual escrow wallets** per trade (payment escrow + affiliate commission escrow)
- **Order book discovery** via GunDB with temporal validation (Pen DSL)
- **Affiliate commission** paid automatically on successful trades

---

## 1. Roles and Trust Model

| Role | Symbol | Description |
|---|---|---|
| **Platform** | **P** | Marketplace operator. Holds authority to release or refund escrow funds. Acts as trusted arbiter in disputes. |
| **Maker** | **M** | Trader who creates an order (buy or sell). |
| **Taker** | **T** | Trader who accepts/matches an existing order. |
| **Affiliate** | **A** | Referrer who brought the payer to the platform. Earns commission on successful trades. |

**Note:** M and T are symmetric — both are traders. A trader can be M in one trade and T in another. The distinction only matters during a specific trade execution. A is optional — if payer has no referrer, no affiliate commission is paid.

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
1. Maker creates order → writes to Gun with Pen-validated key
2. Taker discovers order → Gun .map() query
3. Taker accepts order → trade status: "open" → "matched"
4. Payment deposited to escrow:
   - If buy order: Maker already deposited → Taker confirms receipt
   - If sell order: Taker deposits → Maker delivers item
5. In-game item delivery (trade/mail/drop)
6. Recipient confirms → Platform releases escrow
7. Trade status: "matched" → "completed"
```

### 3.3 Key Derivation Strategy

**Changes from v1.0:**
- **No per-item roots** (items don't need separate xpub trees)
- **One root per trader-platform pair**
- **Two escrow wallets per trade:**
  - **Payment escrow (TE)** — holds trade payment (M/P or T/P root)
  - **Affiliate escrow (AE)** — holds affiliate commission (A/P root)

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

### 4.3 Per-Trade Escrow Wallets (Dual Escrow)

For each matched trade, derive **two escrow wallets**:

#### 4.3.1 Payment Escrow (TE)

Using **payer's root key** (M or T, whoever deposits payment):

```javascript
tradeId     =  sha256(orderId ⊕ M.pub ⊕ T.pub ⊕ timestamp)
seed_TE     =  sha256(secret_MP ⊕ tradeId)   // if M is payer
index_TE    =  index(seed_TE)

TE          =  root_MP.deriveChild(index_TE)
// TE.address — payment escrow wallet
// TE.privateKey — only P can derive
```

#### 4.3.2 Affiliate Escrow (AE)

Using **affiliate's root key** (if payer has referrer):

```javascript
seed_AE     =  sha256(secret_AP ⊕ tradeId)   // using A's secret with P
index_AE    =  index(seed_AE)

AE          =  root_AP.deriveChild(index_AE)
// AE.address — affiliate commission wallet
// AE.privateKey — only P can derive initially
```

**Who pays:**
- **Buy order:** Maker deposits payment + commission → `root_MP` for TE, `root_AP` for AE
- **Sell order:** Taker deposits payment + commission → `root_TP` for TE, `root_AP` for AE

**Access control:**
- **Payer (M or T):** Can compute both `TE.address` and `AE.address` (watch-only via xpub_MP/xpub_TP + xpub_AP)
- **Recipient:** Receives `TE.address` from trade metadata
- **Affiliate (A):** Can compute `AE.address` (watch-only via xpub_AP), receives `index_AE` on trade completion to derive spending key
- **Platform (P):** Can compute full private keys for both TE and AE to release/refund

---

## 5. Protocol Walkthrough

### Step 1 — Maker Creates Order

**Scenario A: Buy Order**
1. M creates buy order on UI
2. M retrieves referrer (if any) from localStorage: `Context.getReferrer()`
3. M computes escrow addresses:
   - `TE.address` via `xpub_MP + index(tradeId)` (payment escrow)
   - `AE.address` via `xpub_AP + index(tradeId)` (commission escrow, if referrer exists)
4. M deposits to escrow wallets:
   - Item payment → `TE.address`
   - Platform fee → Platform operational wallet
   - Affiliate commission → `AE.address` (only if referrer exists)
5. M writes order to Gun with Pen-validated key:
   ```
   <candle>:<item_slug>:buy:<price>:<nonce>
   ```
6. Order status: `"open"`

**Scenario B: Sell Order**
1. M creates sell order (no deposit yet)
2. M writes order to Gun
3. Order status: `"open"`

### Step 2 — Taker Discovers and Accepts Order

1. T queries Gun order book:
   ```javascript
   gun.get(orderSoul).map({
       '.': { '*': `${candle}:penitent-greaves:buy:` }
   })
   ```
2. T sees M's order, clicks "Accept"
3. Trade record created with `tradeId`

**If M created buy order:**
- T verifies `TE.address` has funds on-chain
- T verifies `AE.address` has commission (if applicable)
- Trade status → `"matched"`

**If M created sell order:**
- T retrieves referrer (if any) from localStorage
- T computes escrow addresses:
  - `TE.address` via `xpub_TP + index(tradeId)`
  - `AE.address` via `xpub_AP + index(tradeId)` (if referrer exists)
- T deposits to escrow wallets:
  - Item payment → `TE.address`
  - Platform fee → Platform operational wallet
  - Affiliate commission → `AE.address` (only if referrer exists)
- Trade status → `"matched"`

### Step 3 — Item Delivery

**Seller (whoever has the item) delivers in-game:**
- Trade window in-game
- In-game mail system
- Drop/pickup coordination

**Seller marks:** "Delivered" in UI → updates Gun trade record

### Step 4 — Buyer Confirms Receipt

**Buyer marks:** "Received" in UI

**Resolution logic:**

**Payment release to seller:**
```javascript
// Buyer transmits index_TE to Seller
const secret_payer = await sea.secret(P.pub, payer.pair)
const index_TE = index(sha256(secret_payer + ":" + tradeId))

// Seller recomputes payment escrow wallet
const TE = root_payer.deriveChild(index_TE)

// Seller self-releases payment
const tx = await TE.sendTransaction({
    to: seller.address,
    value: paymentAmount
})
```

**Affiliate commission release:**
```javascript
// Platform transmits index_AE to Affiliate (via Gun)
const secret_AP = await sea.secret(P.pub, A.pair)
const index_AE = index(sha256(secret_AP + ":" + tradeId))

// Affiliate recomputes commission escrow wallet
const AE = root_AP.deriveChild(index_AE)

// Affiliate self-releases commission
const tx = await AE.sendTransaction({
    to: affiliate.address,
    value: commissionAmount
})
```

**OR via Platform auto-release:**
- If buyer doesn't confirm within 24h → P auto-releases both TE and AE

### Step 5 — Dispute Resolution by P

P can always recompute both escrow spending keys:

```javascript
// P knows all parties' pub keys
const secret_payer = await sea.secret(payer.pub, P.pair)
const root_payer = fromSeed(sha256(secret_payer))

const secret_TE = sha256(secret_payer + ":" + tradeId)
const TE = root_payer.deriveChild(index(secret_TE))
// TE.privateKey → P can release or refund payment

// If affiliate exists:
const secret_AP = await sea.secret(A.pub, P.pair)
const root_AP = fromSeed(sha256(secret_AP))

const secret_AE = sha256(secret_AP + ":" + tradeId)
const AE = root_AP.deriveChild(index(secret_AE))
// AE.privateKey → P can release or refund commission
```

**Resolution matrix:**

| Scenario | TE (Payment) Action | AE (Commission) Action |
|---|---|---|
| Trade completed successfully | Release to seller | Release index to affiliate (via Gun) |
| Buyer disputes, seller proven wrong | Refund to payer | Refund to payer |
| Buyer disputes, seller proven right | Release to seller | Release index to affiliate |
| Both parties inactive 7+ days | Refund to payer | Refund to payer |

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
                 match: { type: "string" } } },
        
        // Order type: buy or sell
        { seg: { sep: ":", idx: 2, of: { reg: 0 },
                 match: { or: [{ eq: "buy" }, { eq: "sell" }] } } },
        
        // Price (numeric)
        { seg: { sep: ":", idx: 3, of: { reg: 0 },
                 match: { type: "number" } } }
    ]},
    val: { type: "string" },  // JSON order metadata
    sign: true,               // Require signature
    pow: { field: 1, difficulty: 2 }  // Anti-spam PoW
})
```

### 6.2 Key Format

```
<candle>:<item_slug>:<type>:<price>:<nonce>

Example buy:  5820000:penitent-greaves:buy:100:a3f7b2
Example sell: 5820000:penitent-greaves:sell:95:x8k2m1
```

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

### 7.1 Access Control Guarantees

| Property | Mechanism |
|---|---|
| P holds full escrow authority | P knows all pub keys → recomputes all DH secrets → derives all root xprv and child spending keys for both TE and AE |
| Payer cannot withdraw early | Payer has xpub only; without `index_TE` (derived from secret), child key is unknowable |
| Recipient cannot claim early | Recipient has no key material until index released by buyer or P |
| Affiliate cannot claim prematurely | A has xpub_AP but not `index_AE`; receives index only after successful trade completion |
| No third-party involvement | All derivations local and deterministic |

### 7.2 Per-Trade Address Isolation

Each `tradeId` is unique → `index_TE` and `index_AE` unique → escrow addresses never collide across trades.

Even if same affiliate refers multiple trades, each trade gets unique AE wallet (different `tradeId` → different `index_AE`).

### 7.3 Order Book Spam Prevention

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

- [ ] `src/core/Escrow.js` — key derivation functions (TE + AE)
- [ ] `src/core/Order.js` — Gun order CRUD helpers
- [ ] `src/core/Affiliate.js` — referral tracking + commission calculation
- [ ] `src/UI/routes/order/` — order book UI + trade flow
- [ ] Pen soul definition for orders
- [ ] Gun .map() discovery queries
- [ ] Auto-release 24h timer (both TE and AE)
- [ ] Affiliate index release mechanism (via Gun)
- [ ] Dispute UI at `/dispute`
- [ ] Test suite: dual escrow address derivation consistency
- [ ] Test suite: `sea.secret` determinism
- [ ] Test suite: candle window validation
- [ ] Test suite: affiliate commission calculation

---

*2026-04-04 — Revised for P2P trading model*
