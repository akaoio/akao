# P2P Trading Platform Protocol

**Version** 2.0 · **Status** Production · **Date** 2026-04-06

> **Supersedes**: v1.0 eCommerce model (deprecated - see `white-paper-v1-ecommerce-deprecated.md`)
>
> **Philosophy**: DEX-inspired P2P trading where all participants are symmetric traders. Platform provides infrastructure, not custody.

---

## Abstract

This document specifies a **trustless, serverless platform protocol for peer-to-peer trading** of digital assets (game items, NFTs, digital goods). The protocol enables a platform operator to mediate trades between two traders (maker and taker) using deterministic cryptographic key derivation — eliminating the need for smart contracts, custodial accounts, or trusted third-party oracles.

All platform wallet addresses are derived client-side from **Diffie-Hellman shared secrets** and **BIP-32 hierarchical deterministic key derivation**, ensuring that each party holds exactly the access rights the protocol intends, and no more.

### Think DEX, Not Marketplace

**Traditional marketplace** (v1.0 - deprecated):
- ❌ Fixed roles: Buyer pays, Seller sells
- ❌ Item-centric: Each item has separate platform tree
- ❌ One-directional: Only buyers initiate trades
- ❌ Custodial mindset: Platform holds listings

**DEX/Trading mindset** (v2.0 - current):
- ✅ **Symmetric roles**: Anyone can create buy OR sell orders
- ✅ **Trader-centric**: Maker/Taker like Uniswap, dYdX, Binance
- ✅ **Bi-directional**: Orders are just intentions (buy intention = sell intention from other side)
- ✅ **Order book**: Decentralized discovery via Gun + Pen validation, partitioned by item soul and order-side soul
- ✅ **Affiliate economy**: Built-in referral commissions
- ✅ **Non-custodial (happy path)**: Platform arbitrates disputes/refunds, not involved in normal trades

**Key innovations**:
- **Dual platform wallets** per trade: Transaction Lock (TL) + Commission Lock (CL)
- **Temporal validation**: Orders auto-expire using candle-based epochs (Pen DSL)
- **Post-match deposits**: Both order types deposit after matching (pure P2P, no Platform involvement)
- **Trustless verification**: Anyone can verify platform addresses on-chain before accepting

---

## Glossary of Abbreviations

| Abbreviation | Full Name | Description |
|--------------|-----------|-------------|
| **P** | Platform | Marketplace operator and trusted arbiter |
| **M** | Maker | Trader who creates an order (buy or sell) |
| **T** | Taker | Trader who accepts/matches an order |
| **A** | Affiliate | Referrer who earns commission on trades |
| **FP** | Fund Proof | Pre-deposit wallet for buy orders (proof of funds) |
| **TL** | Transaction Lock | Platform wallet holding trade payment |
| **CL** | Commission Lock | Platform wallet holding affiliate commission |
| **SEA** | Security, Encryption, Authorization | GunDB's cryptography library (Curve25519) |
| **BIP-32** | Bitcoin Improvement Proposal 32 | Hierarchical deterministic wallet standard |
| **ECDH** | Elliptic Curve Diffie-Hellman | Shared secret key exchange protocol |
| **HD** | Hierarchical Deterministic | Wallet derivation system (BIP-32) |
| **xpub** | Extended Public Key | Watch-only public key for address derivation |
| **xprv** | Extended Private Key | Root private key for spending |
| **DSL** | Domain-Specific Language | Pen validation language for order keys |

---

## 1. Roles and Trust Model

### 1.1 Parties

| Role | Symbol | Description |
|---|---|---|
| **Platform** | **P** | Marketplace operator. Holds authority to release or refund platform funds. Acts as trusted arbiter in disputes. |
| **Maker** | **M** | Trader who creates an order (buy or sell). |
| **Taker** | **T** | Trader who accepts/matches an existing order. |
| **Affiliate** | **A** | Referrer who brought the payer to the platform. Earns commission on successful trades. |

**Note:** M and T are symmetric — both are traders. A trader can be M in one trade and T in another. The distinction only matters during a specific trade execution. A is optional — if payer has no referrer, no affiliate commission is paid.

### 1.2 Platform Wallets

| Wallet | Symbol | Full Name | Purpose | Derived From |
|--------|--------|-----------|---------|--------------|
| Fund Proof | **FP** | Fund Proof | Pre-deposit for buy orders (proof of funds) | `root_M` (Maker's own root) |
| Payment platform | **TL** | Transaction Lock | Holds trade payment (payer → recipient) | `root_MP` or `root_TP` (recipient's root) |
| Commission platform | **CL** | Commission Lock | Holds affiliate commission (payer → affiliate) | `root_AP` (affiliate's root) |

**Notes:** 
- **FP** (buy orders only): Maker deposits before posting order → Taker verifies on-chain → funds transfer to TL/CL after matching
- Each trade creates exactly 2 lock wallets: 1 TL (payment) + 1 CL (commission, if affiliate exists)
- FP uses **maker's own root** (maker controls funds), TL/CL use **recipient's root** (trustless platform)

### Trust Assumptions

- **P is trusted** to act honestly as arbiter (semi-centralized model)
- **No party other than P** can unilaterally move funds from platform before trade resolution
- **Both traders trust P** to resolve disputes fairly
- All parties have valid **SEA key pairs** registered via WebAuthn

---

## 2. Cryptographic Primitives

Same as v1.0 (no changes):

### 2.1 ECDH Shared Secret — `sea.secret`

`sea.secret(theirPublicKey, myKeyPair)` over Curve25519 (GunDB SEA library).

**Properties:**
- **Symmetric:** `sea.secret(P.epub, M.pair) === sea.secret(M.epub, P.pair)`
- **Deterministic:** Same inputs → same output
- **Pair-scoped:** `secret_MP ≠ secret_TP`

```javascript
const secret_MP = await sea.secret(P.epub, M.pair)  // M computes
const secret_MP = await sea.secret(M.epub, P.pair)  // P computes (same result)
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

### 3.1 Order Sides

**Buy Order:**
```javascript
{
    side: "buy",
    maker: {
        pub: "maker_pub_key",
        epub: "maker_epub_key",
        xpub: "maker_root_xpub"
    },
    base: {
        type: "item",
        id: "diablo-4/penitent-greaves-d76bc",
        quantity: 1
    },
    quote: {
        type: "crypto",
        quantity: 100,
        contract: "USDT",
        chain: 1
    },
    status: "open"
}
```

**Sell Order:**
```javascript
{
    side: "sell",
    maker: {
        pub: "maker_pub_key",
        epub: "maker_epub_key",
        xpub: "maker_root_xpub"
    },
    base: {
        type: "item",
        id: "diablo-4/penitent-greaves-d76bc",
        quantity: 1
    },
    quote: {
        type: "crypto",
        quantity: 95,
        contract: "USDT",
        chain: 1
    },
    status: "open"
}
```

### 3.2 Trade Execution Flow

```
1. Maker creates order → writes to the Pen soul compiled for the exact `(baseId, side, candle)` market window (no deposit)
2. Taker discovers order → compiles the current and previous `(baseId, side, candle)` souls, then scans both
3. Taker accepts order → trade status: "open" → "matched" (both parties known)
4. Payer deposits to platform (Maker for buy, Taker for sell):
   - Computes TL/CL from recipient's xpub + payer's index
   - Deposits within 10-min timeout or trade auto-cancels
5. In-game item delivery (trade/mail/drop)
6. Buyer confirms → reveals index to seller/affiliate
7. Recipients withdraw using own root + revealed index
8. Trade status: "matched" → "deposited" → "delivered" → "completed"
```

### 3.3 Key Derivation Strategy

**Changes from v1.0:**
- **No per-item platform roots** (items don't need separate xpub trees for settlement)
- **Per-item, per-type, per-candle Pen souls** for discovery (`item + side + time window` define the soul identity)
- **One root per trader-platform pair**
- **Two platform wallets per trade:**
  - **Payment platform (TL)** — holds trade payment (M/P or T/P root)
  - **Affiliate platform (CL)** — holds affiliate commission (A/P root)

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
secret_MP  =  sea.secret(P.epub, M.pair)
secret_TP  =  sea.secret(P.epub, T.pair)
secret_AP  =  sea.secret(P.epub, A.pair)

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

### 4.3 Per-Transaction Lock Wallets (Dual Platform)

**Key principle from original design**: Platform wallet = **RECIPIENT's root xpub** + **INDEX from PAYER's secret**

**Critical design decision**: Both buy and sell orders deposit **AFTER matching** to preserve pure P2P architecture (Platform not involved in happy path).

For each trade, derive **two platform wallets**:

#### 4.3.1 Transaction Lock (TL)

**Symmetric flow for both order types** (Maker always uses Taker's xpub after match):

```javascript
// AFTER MATCHING (both parties known)
// "TR:" domain separator + ":" field separators prevent hash space collision with orderId ("OR:")
tradeId = sha256("TR:" + orderId + ":" + M.pub + ":" + T.pub)

// Step 1: Recipient (Taker for buy order, Maker for sell order) publishes xpub
secret_recipient = sea.secret(P.epub, recipient.pair)
root_recipient = fromSeed(sha256(secret_recipient))
xpub_recipient = root_recipient.neuter().extendedKey  // Public

// Step 2: Payer (Maker for buy order, Taker for sell order) computes index
// Domain separator ":TL:" isolates TL index from CL index — prevents cross-lock correlation
secret_payer = sea.secret(P.epub, payer.pair)
seed_index = sha256(secret_payer + ":TL:" + tradeId)
index_TL = parseInt(seed_index.slice(0,8), 16) & 0x7fffffff

// Step 3: Payer derives platform from RECIPIENT's xpub + PAYER's index
TL = HDNodeWallet.fromExtendedKey(xpub_recipient).deriveChild(index_TL)
//   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Recipient's root (watch-only)
//                                               ^^^^^^^^^ Payer's index

// TL.address — Payer deposits HERE (after matching)
// TL.privateKey — Payer doesn't have (only has xpub, not xprv)
// Recipient needs index_TL revealed to derive spending key
```

**Who is payer/recipient?**
- **Buy order**: Maker (buyer) is payer, Taker (seller) is recipient
- **Sell order**: Taker (buyer) is payer, Maker (seller) is recipient

#### 4.3.2 Commission Lock (CL)

**Same pattern - payer uses affiliate's xpub + payer's index:**

```javascript
// AFTER MATCHING (payer known)
tradeId = sha256("TR:" + orderId + ":" + M.pub + ":" + T.pub)

// Affiliate publishes xpub
secret_AP = sea.secret(P.epub, A.pair)
root_AP = fromSeed(sha256(secret_AP))
xpub_AP = root_AP.neuter().extendedKey

// Payer computes index from own secret
// Domain separator ":CL:" isolates CL index from TL index — prevents cross-lock correlation
secret_payer = sea.secret(P.epub, payer.pair)
seed_index = sha256(secret_payer + ":CL:" + tradeId)
index_CL = parseInt(seed_index.slice(0,8), 16) & 0x7fffffff

// Payer derives from AFFILIATE's xpub + PAYER's index
CL = HDNodeWallet.fromExtendedKey(xpub_AP).deriveChild(index_CL)

// CL.address — payer deposits commission here (after matching)
// CL.privateKey — Affiliate needs index revealed to spend
```

**Security properties**:
- Payer derives platform address from **recipient's xpub** (watch-only)
- Payer uses index from **own secret** (knows index)
- Payer **cannot spend** (doesn't have recipient's xprv, only xpub)
- Recipient **cannot spend yet** (doesn't know index)
- Platform **can always spend** (knows all secrets → can compute all indexes + has all xprvs)

**Who pays (AFTER matching):**
- **Buy order:** Maker deposits to `TL` + `CL` (both use Taker's/Affiliate's xpub + Maker's index)
- **Sell order:** Taker deposits to `TL` + `CL` (both use Maker's/Affiliate's xpub + Taker's index)

**When deposit happens:**
- **Both order types**: Deposit occurs AFTER matching (when both parties known)
- No pre-deposit required → Platform not involved in happy path → pure P2P

**Access control:**
- **Payer (M or T):** Computes platform addresses from recipient's xpub + own index. **Cannot spend** (has xpub only, not xprv).
- **Recipient (seller):** 
  - Buy order: Taker receives `index_TL` from Maker → derives spending key from own `root_TP`
  - Sell order: Maker receives `index_TL` from Taker → derives spending key from own `root_MP`
- **Affiliate (A):** Receives `index_CL` from payer (or Platform) → derives spending key from own `root_AP`
- **Platform (P):** Knows all secrets → can compute all indexes + derive all spending keys (arbitration power)

**Unlock mechanism**:
```
BOTH ORDER TYPES FOLLOW SAME FLOW (symmetric):

1. Maker posts order (no deposit)
2. Taker accepts → tradeId created
3. Payer computes TL/CL from Recipient's xpub + Payer's index
4. Payer deposits to TL.address + CL.address (within deposit timeout)
5. Seller delivers item
6. Buyer confirms receipt → reveals index_TL and index_CL
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

**Original security mindset preserved**: Platform wallet = recipient's root + payer's index

---

## 5. Protocol Walkthrough

### Step 1 — Maker Creates Order

**Both order types follow same pattern** (symmetric design):

1. M creates order on UI (buy or sell)
2. M generates `orderId = sha256("OR:" + canonicalOrderIntent + ":" + timestamp + ":" + randomness)`
3. M retrieves affiliate (if any) from localStorage: `Context.getReferrer()`
4. M compiles the Pen soul for the exact `(baseId, side, candle)` window, then writes under a Pen-validated key:
   ```
   soul params: { baseId: <base_item_id>, side: <side>, candle: <candle> }
   key:         <timestamp_ms>:<pub>:<nonce>

   Example soul params: { baseId: "penitent-greaves", side: "buy", candle: 5820000 }
   Example key:         1744440123456:maker_full_pub:x8k2m1
   ```
   `pub` is the maker's full public key, not a truncated shard
5. Order includes metadata: `{ orderId, maker, side, base, quote, affiliate }`
6. Order status: `"open"` (no funds locked yet, waiting for Taker)

**No pre-deposit** for either order type → pure P2P (Platform not involved)

### Step 1.5 — Order Wallet (Proof of Funds) for Buy Orders

**Problem**: Takers waste time accepting buy orders from makers who don't actually have funds.

**Solution**: Buy order makers deposit funds into a **deterministic order wallet** before creating the order. This provides **trustless proof of budget** without locking funds in platform yet.

**Order Wallet Derivation** (buy orders only):

```javascript
// Maker derives order wallet from own xpub + orderId
const secret_M = await sea.secret(P.epub, M.pair)
const root_M = HDNodeWallet.fromSeed(sha256(secret_M))
const xpub_M = root_M.neuter().extendedKey

// Order-specific index — "FP:" domain separator isolates FP address space from TL/CL on same root
const index_order = parseInt(sha256("FP:" + orderId).slice(0,8), 16) & 0x7fffffff

// Fund Proof (Maker controls private key)
const orderWallet = root_M.deriveChild(index_order)

// Maker deposits BEFORE posting order
await M.wallet.sendTransaction({
    to: orderWallet.address,
    value: totalAmount  // payment + commission + platform fee
})
```

**Order creation flow (buy orders)**:
1. Maker computes `orderWallet.address` from `xpub_M + orderId`
2. Maker deposits full amount to `orderWallet.address`
3. Maker posts order to Gun with `orderWallet: orderWallet.address`
4. Taker verifies on-chain balance before accepting: `chain.getBalance(orderWallet.address) >= requiredAmount`

**After matching** (Taker accepts):
- Maker transfers from `orderWallet` → Transaction Lock (TL) + Commission Lock (CL)
- ⚠️ **Not atomic**: these are sequential EVM transactions (TL tx, CL tx, platform fee tx). If any fails mid-way, trade is in partial state. Implementation must check each receipt and handle partial completion.
- Gas cost: up to 3 transactions (TL + CL + platform fee)

**Benefits**:
- ✅ Taker can verify funds exist on-chain before accepting
- ✅ Trustless proof (on-chain balance query)
- ✅ Maker retains full control of funds in order wallet (can cancel order and withdraw anytime)
- ✅ No Platform involvement until trade is matched

**Cancellation**:
- Maker withdraws from `orderWallet` back to main wallet using own private key
- Order auto-expires when balance drops below required amount

**Sell orders**: No order wallet needed (seller proves ownership by delivering item, not by depositing funds upfront)

---

### Step 2 — Taker Discovers and Accepts Order

1. T compiles the exact market-window souls in Gun:
    ```javascript
    const current = Order.soul({ baseId: "penitent-greaves", side: "buy", candle })
    const previous = Order.soul({ baseId: "penitent-greaves", side: "buy", candle: candle - 1 })
    ```
2. T scans both souls:
    ```javascript
    gun.get(current).map()
    gun.get(previous).map()
    ```
3. T sees M's order
    - **For buy orders**: T verifies on-chain balance of `orderWallet.address` before accepting
      ```javascript
      const balance = await chain.getBalance(order.orderWallet)
     if (balance < requiredAmount) {
         UI.error("Maker has insufficient funds")
         return
     }
     ```
   - **For sell orders**: No verification needed (seller will prove ownership by delivery)
4. T clicks "Accept"
5. **Trade matching** (creates `tradeId`):
   ```javascript
   // tradeId computed once at match time, stored in Gun trade record — never recomputed
   // "TR:" domain separator prevents hash space collision with orderId ("OR:")
   tradeId = sha256("TR:" + orderId + ":" + M.pub + ":" + T.pub)
   ```
6. Trade status → `"matched"` (both parties known, waiting for deposit)

### Step 3 — Payer Deposits to Platform

**Payer** (Maker for buy order, Taker for sell order) now computes platform addresses:

```javascript
// Determine payer and recipient based on order type
const payer = (orderType === 'buy') ? Maker : Taker
const recipient = (orderType === 'buy') ? Taker : Maker

// Get recipient's xpub (published in their profile)
const xpub_recipient = await gun.user(recipient.pub).get('xpub')

// Get affiliate's xpub from signed user namespace — MUST use gun.user(affiliate.pub)
// Never fetch xpub from an arbitrary path — a tampered xpub would redirect funds to attacker's tree
const referrer = payer.getReferrer()  // returns affiliate's pub key
const xpub_affiliate = referrer ? await gun.user(referrer).get('xpub') : null

// Compute payer's index — separate domain separators prevent TL/CL address correlation
const secret_payer = await sea.secret(P.epub, payer.pair)
const index_TL = parseInt(sha256(secret_payer + ":TL:" + tradeId).slice(0,8), 16) & 0x7fffffff
const index_CL = parseInt(sha256(secret_payer + ":CL:" + tradeId).slice(0,8), 16) & 0x7fffffff

// Derive Transaction Lock from recipient's xpub + payer's TL index
const TL = HDNodeWallet.fromExtendedKey(xpub_recipient).deriveChild(index_TL)

// Derive Commission Lock (if affiliate exists)
const CL = referrer ? 
    HDNodeWallet.fromExtendedKey(xpub_affiliate).deriveChild(index_CL) : 
    null

// Payer deposits to platform locks
// For buy orders: Transfer from orderWallet → TL/CL (sequential, NOT atomic — check each tx receipt)
// For sell orders: Direct deposit from Taker's wallet → TL/CL

if (orderType === 'buy') {
    // Maker transfers from order wallet to platform locks
    const orderWallet = M.root.deriveChild(index_order)  // Maker has private key
    
    await orderWallet.sendTransaction({ to: TL.address, value: paymentAmount })
    if (CL) {
        await orderWallet.sendTransaction({ to: CL.address, value: commissionAmount })
    }
    await orderWallet.sendTransaction({ 
        to: Platform.operationalWallet, 
        value: platformFee 
    })
} else {
    // Taker deposits directly to platform locks
    await T.wallet.sendTransaction({ to: TL.address, value: paymentAmount })
    if (CL) {
        await T.wallet.sendTransaction({ to: CL.address, value: commissionAmount })
    }
    await T.wallet.sendTransaction({ 
        to: Platform.operationalWallet, 
        value: platformFee 
    })
}
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
// Buyer computes TL and CL indexes from own secret — domain separators match deposit step
const secret_buyer = await sea.secret(P.epub, buyer.pair)
const index_TL = parseInt(sha256(secret_buyer + ":TL:" + tradeId).slice(0,8), 16) & 0x7fffffff
const index_CL = parseInt(sha256(secret_buyer + ":CL:" + tradeId).slice(0,8), 16) & 0x7fffffff

// Buyer writes to OWN Gun user namespace — Gun SEA enforces only buyer can write here
// Seller subscribes to gun.user(buyer.pub).get("trades").get(tradeId)
gun.user().get("trades").get(tradeId).put({
    unlock_index_TL: index_TL,
    unlock_index_CL: index_CL,
    confirmed: true
})

// Seller receives index and unlocks using OWN root
const secret_seller = await sea.secret(P.epub, seller.pair)
const root_seller = HDNodeWallet.fromSeed(sha256(secret_seller))
const TL = root_seller.deriveChild(index_TL)  // Now has private key!

// Seller self-releases payment
await TL.sendTransaction({
    to: seller.wallet,
    value: paymentAmount
})

// Affiliate (if exists) self-releases commission using CL index (not TL index)
if (affiliate) {
    const secret_affiliate = await sea.secret(P.epub, affiliate.pair)
    const root_affiliate = HDNodeWallet.fromSeed(sha256(secret_affiliate))
    const CL = root_affiliate.deriveChild(index_CL)
    
    await CL.sendTransaction({
        to: affiliate.wallet,
        value: commissionAmount
    })
}
```

Trade status → `"completed"`

**OR via Platform auto-release:**
- If buyer doesn't confirm within 24h → P computes indexes and releases to seller + affiliate

### Step 6 — Dispute Resolution and Refunds by P

**Critical limitation**: Refunds REQUIRE Platform involvement due to platform design.

**Why payer cannot self-refund:**
```javascript
// Platform = Recipient's xpub + Payer's index
TL = HDNodeWallet.fromExtendedKey(xpub_recipient).deriveChild(index_payer)

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

P can always recompute both platform spending keys:

```javascript
// P knows all parties' epub keys (Curve25519) — never .pub (Ed25519)
const secret_recipient = await sea.secret(recipient.epub, P.pair)
const root_recipient = fromSeed(sha256(secret_recipient))

// P recomputes payer's TL and CL indexes using domain separators
const secret_payer = await sea.secret(payer.epub, P.pair)
const index_TL = parseInt(sha256(secret_payer + ":TL:" + tradeId).slice(0,8), 16) & 0x7fffffff
const index_CL = parseInt(sha256(secret_payer + ":CL:" + tradeId).slice(0,8), 16) & 0x7fffffff

// P derives both platform spending keys
const TL = root_recipient.deriveChild(index_TL)
// TL.privateKey → P can release to seller OR refund to payer

// Same for affiliate lock
const secret_affiliate = await sea.secret(affiliate.epub, P.pair)
const root_affiliate = fromSeed(sha256(secret_affiliate))
const CL = root_affiliate.deriveChild(index_CL)
// CL.privateKey → P can release to affiliate OR refund to payer
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

| Scenario | TL (Payment) Action | CL (Commission) Action |
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

Orders use **parameterized Pen souls** and **timestamp keys**:

```javascript
const orderSoul = SEA.pen({
    key: { and: [
        // seg 0: timestamp_ms whose derived candle must equal the soul candle
        {
            let: {
                bind: 0,
                def: { divu: [{ tonum: { seg: { sep: ":", idx: 0, of: { reg: 0 } } } }, 300000] },
                body: { and: [
                    { gte: [{ reg: 128 }, candle] },
                    { lte: [{ reg: 128 }, candle] }
                ]}
            }
        },
        // seg 1: full writer pub must equal R[5]
        { eq: [
            { seg: { sep: ":", idx: 1, of: { reg: 0 } } },
            { reg: 5 }
        ]},
        // seg 2: nonce — iterated by client for PoW
        { seg: { sep: ":", idx: 2, of: { reg: 0 }, match: { length: [1, 64] } } }
    ]},
    val: { type: "string" },  // JSON order metadata (orderId, maker, side, base, quote, affiliate, etc.)
    sign: true,
    pow: { field: 0, difficulty: 3 },  // Anti-spam PoW — client iterates nonce at seg 2
    params: { baseId, side, candle }
})
```

**Important partitioning rule**:
- **Soul identity carries market window**: `params = { baseId, side, candle }`
- **Key carries order instance identity**: `<timestamp>:<pub>:<nonce>`
- **Read path starts from the right soul**, not from a global order bucket

### 6.2 Key Format

```text
soul params: { baseId: <base_item_id>, side: <side>, candle: <candle> }
key:         <timestamp_ms>:<pub>:<nonce>

Example params: { baseId: "penitent-greaves", side: "buy", candle: 5820000 }
Example key:    1744440123456:maker_full_pub:x8k2m1
```

| Segment | Content | Validated by |
|---|---|---|
| soul | `params = { baseId, side, candle }` | Compile-time soul identity via `SEA.pen({ params })` |
| seg 0 | `timestamp_ms = Date.now()` | `floor(timestamp_ms / 300000) === candle` |
| seg 1 | `pub = M.pub` | Dynamic equality with writer register `R[5]` |
| seg 2 | `nonce` — client iterates until PoW passes | PoW `difficulty: 3` |

**orderId** is stored in the value (not the key), enabling:
- Stable identity independent of timestamp/nonce
- Price changes without key recreation
- Matching/cancellation by key while market window stays derivable from timestamp

**Candle calculation:**
```javascript
const candle = Math.floor(Date.now() / 300000)  // 5-minute candles

// Full key construction inside soul({ baseId, side, candle }):
const timestamp = Date.now()
const pub = pair.pub
const nonce = await computePowNonce(`${timestamp}:${pub}`, 3)
const key = `${timestamp}:${pub}:${nonce}`
```

**Benefits:**
- Soul identity is exact to one market window
- Full pub is stronger than `pub8` for ownership and cancellation checks
- Discovery queries start from exact baseId/side/candle souls
- No manual cleanup needed
- Market noise from other items, sides, or candles does not pollute discovery

---

### 6.3 Trade Record Soul

Trade records contain sensitive data (`unlock_index_TL`, `unlock_index_CL`). No shared Pen soul is appropriate. Each party writes to their **own Gun user namespace**, keyed by `tradeId`.

**Two write targets per trade:**

```javascript
gun.user(M.pub).get("trades").get(tradeId)   // Maker writes here — Gun rejects other writers
gun.user(T.pub).get("trades").get(tradeId)   // Taker writes here — Gun rejects other writers
```

Gun SEA enforces write restriction via signature — **no Pen needed**.

**Who writes what:**

| Field | Writer | Namespace | When |
|---|---|---|---|
| `matched` (timestamp) | Taker | Taker's | Step 2 — at accept |
| `deposited` (txHash) | Payer (M or T) | Own | Step 3 — after deposit confirmed |
| `delivered` (true) | Seller (M or T) | Own | Step 4 — item delivered |
| `unlock_index_TL` (number) | Buyer (M or T) | Own | Step 5 — at confirmation |
| `unlock_index_CL` (number) | Buyer (M or T) | Own | Step 5 — at confirmation |
| `confirmed` (true) | Buyer (M or T) | Own | Step 5 — receipt confirmed |
| `disputed` (object) | Either party | Own | Step 6 — if dispute filed |
| `release_ready` / `refund_ready` | Platform | Platform's | Step 6 — platform has resolved deterministic spend paths but has not settled on-chain yet |

**Key security property**: `unlock_index_TL` and `unlock_index_CL` live in the **buyer's own namespace**. Seller must actively read buyer's namespace to obtain the index. Buyer controls when seller can withdraw — no premature self-release is possible.

**Read pattern — both parties subscribe to both namespaces:**

```javascript
gun.user(M.pub).get("trades").get(tradeId).on(makerData => merge(makerData))
gun.user(T.pub).get("trades").get(tradeId).on(takerData => merge(takerData))
```

**`Trade.js` soul mapping:**

```javascript
trade.id()     →  tradeId                               // content hash — used in crypto derivation
trade.soul()   →  { maker: M.pub, taker: T.pub }        // keys to compose Gun paths

// Write to own namespace (caller authenticated as writer)
gun.user().get("trades").get(tradeId).put(fields)

// Read from both namespaces
gun.user(M.pub).get("trades").get(tradeId)              // Maker's side
gun.user(T.pub).get("trades").get(tradeId)              // Taker's side
```

**Platform access**: Platform can read both namespaces (they're public Gun user data). Platform writes to trade records via the trade participant's shared secret — Platform knows all parties' epub keys and can compute all secrets.

---

## 7. Security Analysis

### 7.1 The Refund Limitation

**Critical design constraint**: Payer cannot self-refund in the current platform model.

**Why this limitation exists:**

The platform security relies on **asymmetric key access**:
```
Platform = Recipient's xpub (watch-only) + Payer's index (secret)

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
| P holds full platform authority | P knows all **epub** keys (Curve25519) → recomputes all DH secrets via `sea.secret(party.epub, P.pair)` → derives all root xprv and child spending keys for both TL and CL |
| Payer cannot withdraw (even for refund) | Payer has recipient's xpub only (watch-only). Cannot derive private key from xpub. **Refunds require Platform intervention.** |
| Recipient cannot claim early | Recipient has xprv but doesn't know index until payer reveals it |
| Affiliate cannot claim prematurely | A has xpub_AP but not `index_CL`; receives index only after successful trade completion |
| No third-party involvement (happy path) | All derivations local and deterministic. Platform only involved for disputes/refunds. |

### 7.3 Per-Trade Address Isolation

Each `tradeId` is unique → `index_TL` and `index_CL` unique → platform addresses never collide across trades.

Even if same affiliate refers multiple trades, each trade gets unique CL wallet (different `tradeId` → different `index_CL`).

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
| xpub tampering in Gun | Medium | xpub stored in `gun.user(pub)` — only owner can write (SEA-signed). Platform must validate xpub matches expected derivation before allowing deposit phase. |
| Crafted/wrong xpub by recipient | **High** | Recipient could write xpub for which they (and Platform) have no xprv → funds permanently locked in TL. **Platform MUST verify `xpub_MP` is derived from `sha256(sea.secret(P.epub, M.pair))`** before trade proceeds to deposit. |
| Order ID collision | Very low | `sha256(orderId ":" M.pub ":" T.pub ":" timestamp)` |
| Order overwrite attack | High | Pen `sign:true` allows any authenticated user to overwrite order keys at PoW cost ~256 attempts. Mitigate: embed maker pub prefix in key and validate `EQ(SEGR(0,':',4), R[5])` in Pen — or move orders to `gun.user()` namespace. |
| FP race condition | High | Maker can drain FP wallet between Taker accepting and payer depositing to TL/CL. Design limitation of non-smart-contract platform. Taker should re-verify FP balance at matching time, not just at discovery time. |
| Partial FP→TL/CL transfer | High | Sequential EVM transactions mean TL could be funded but CL fails. Trade contract must check all receipts; implementation needs a recovery path (refund from partial TL if CL fails). |
| Commission fate with no affiliate | Medium | When referrer is null, `commissionAmount` is unaccounted for in current protocol. Must define: goes to platform fee, added to payment, or simply not charged. |
| Child key leakage in BIP-32 | Medium | Non-hardened derivation: `child_priv + xpub → parent_priv`. Spending keys (TL, CL) must be wiped from memory immediately after `sendTransaction()`. Never log or store them. |
| P key compromise | Critical | Operational security (outside protocol scope) |
| Payer cannot self-refund | **Design limitation** | **Platform MUST intervene for refunds.** Payer only has recipient's xpub (watch-only), cannot derive private key. This is by design — same mechanism that prevents payer from withdrawing prevents self-refund. |
| Platform unavailable (refunds blocked) | Medium | Multi-sig Platform keys, backup arbitrators, or smart contract fallback (future) |
| In-game item delivery fraud | Medium | Dispute resolution by P with proof requirements |
| Candle drift (client time skew) | Low | Accept ±2 candles forward, ±100 backward |
| Trade record soul | ✅ Resolved | Dual user-namespace model (Section 6.3). Each party writes to `gun.user(own.pub).get("trades").get(tradeId)`. Gun SEA enforces authorship. No Pen needed. |
| Order soul overwrite | ✅ Resolved in direction | Full writer pub now lives in key seg 1, and soul identity is parameterized by `{ baseId, side, candle }` via `SEA.pen({ params })` (Section 6.1). |

---

## 9. Out of Scope

- **Fiat payments** (handled by platform account balances)
- **Smart contract enforcement** (intentionally avoided)
- **Multi-signature** (future revision candidate)
- **Cross-chain atomic swaps** (single-chain trades only)
- **In-game API integration** (manual delivery via trade/mail)

---

## 10. Implementation Checklist

- [ ] `src/core/Lock.js` — platform key derivation primitives (TL + CL, `index()`, `address()`, `unlock()`)
- [ ] `src/core/Order.js` — Gun order CRUD helpers
- [ ] `src/core/Affiliate.js` — referral tracking + commission calculation
- [ ] `src/UI/routes/order/` — order book UI + trade flow
- [ ] Pen soul definition for orders
- [ ] Per-baseId + per-side Gun discovery queries with candle range scanning
- [ ] Auto-release 24h timer (both TL and CL)
- [ ] Affiliate index release mechanism (via Gun)
- [ ] Dispute UI at `/dispute`
- [ ] Test suite: dual platform address derivation consistency
- [ ] Test suite: `sea.secret` determinism
- [ ] Test suite: candle window validation
- [ ] Test suite: affiliate commission calculation

---

*2026-04-04 — Revised for P2P trading model*  
*2026-04-06 — Resolved: No pre-deposit for buy orders (pure P2P, symmetric design)*  
*2026-04-08 — Security audit: epub/pub fix, TL/CL domain separators, tradeId separators, FP domain separator, atomic transition caveat, order overwrite risk, crafted xpub risk, child key leakage note, commission gap, trade record soul gap*
